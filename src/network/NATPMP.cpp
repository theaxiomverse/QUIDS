#include "network/NATPMP.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <spdlog/spdlog.h>
#include <boost/asio/ip/network_v4.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/steady_timer.hpp> // Include the timer header

namespace quids::network {

    std::string NATPMP::discover_gateway() {
        try {
            boost::asio::io_context io_context;
            boost::asio::ip::udp::socket socket(io_context);
            socket.open(boost::asio::ip::udp::v4());

            // Connect to a dummy endpoint to get local address
            boost::asio::ip::udp::resolver resolver(io_context);
            auto endpoints = resolver.resolve(
                boost::asio::ip::udp::v4(),
                "8.8.8.8",  // Google DNS as dummy target
                "53"        // DNS port
            );
            socket.connect(*endpoints.begin());
            
            // Get local address
            auto local_addr = socket.local_endpoint().address().to_v4();
            
            // Get network interfaces
            boost::asio::ip::network_v4 network(local_addr, 24); // Assume /24 network
            
          auto gateway = network.network().to_uint() | 0x01;
return boost::asio::ip::address_v4(gateway).to_string(); // This line is correct
            
        } catch (const boost::system::system_error& e) {
            SPDLOG_ERROR("Failed to discover gateway: {}", e.what());
            return "";
        }
    }

    bool NATPMP::map_port(uint16_t internal_port, uint16_t external_port, 
                        Protocol proto, uint32_t lifetime) {
        try {
            std::string gateway = discover_gateway();
            if (gateway.empty()) {
                SPDLOG_ERROR("Failed to discover gateway");
                return false;
            }

            boost::asio::io_context io_context;
            boost::asio::ip::udp::socket socket(io_context);
            socket.open(boost::asio::ip::udp::v4());

            // Create endpoint for NAT-PMP port
            boost::asio::ip::udp::endpoint gateway_endpoint(
                boost::asio::ip::make_address_v4(gateway),
                5351  // NAT-PMP port
            );

            // Create NAT-PMP request
            struct {
                uint8_t version;
                uint8_t opcode;
                uint16_t reserved;
                uint16_t internal_port;
                uint16_t external_port;
                uint32_t lifetime;
            } request{};

            request.version = 0;
            request.opcode = proto == Protocol::TCP ? 2 : 1;
            request.reserved = 0;
            request.internal_port = htons(internal_port);
            request.external_port = htons(external_port);
            request.lifetime = htonl(lifetime);

            // Send request
            socket.send_to(boost::asio::buffer(&request, sizeof(request)), gateway_endpoint);

            // Set timeout for response
            boost::asio::steady_timer timer(socket.get_executor()); // Create a timer
timer.expires_after(std::chrono::seconds(2)); // Set timeout for 2 seconds

// Start the timer
timer.async_wait([this, &socket](const boost::system::error_code& ec) { // Capture socket by reference
    if (!ec) {
        // Handle timeout (e.g., log an error, close the socket, etc.)
        SPDLOG_ERROR("Receive timeout occurred.");
        socket.close(); // Now this will work
    }
});

            // Receive response
            struct {
                uint8_t version;
                uint8_t opcode;
                uint16_t result_code;
                uint32_t epoch;
                uint16_t internal_port;
                uint16_t external_port;
                uint32_t lifetime;
            } response{};

            boost::asio::ip::udp::endpoint sender_endpoint;
            size_t received = socket.receive_from(
                boost::asio::buffer(&response, sizeof(response)),
                sender_endpoint
            );

            if (received < sizeof(response)) {
                SPDLOG_ERROR("Received incomplete NAT-PMP response");
                return false;
            }

            // Verify response
            if (response.version != 0) {
                SPDLOG_ERROR("Invalid NAT-PMP response version");
                return false;
            }

            if (response.opcode != (request.opcode | 0x80)) {
                SPDLOG_ERROR("Invalid NAT-PMP response opcode");
                return false;
            }

            uint16_t result = ntohs(response.result_code);
            if (result != 0) {
                SPDLOG_ERROR("NAT-PMP request failed with code: {}", result);
                return false;
            }

            // Store the mapping details
            mapped_external_port_ = ntohs(response.external_port);
            mapping_lifetime_ = ntohl(response.lifetime);
            last_mapping_time_ = std::chrono::steady_clock::now();
            mapped_internal_port_ = internal_port;
            last_protocol_ = proto;

            return true;

        } catch (const boost::system::system_error& e) {
            SPDLOG_ERROR("NAT-PMP request failed: {}", e.what());
            return false;
        }
    }

    bool NATPMP::refreshMapping() {
        if (mapped_external_port_ == 0) {
            SPDLOG_ERROR("No active mapping to refresh");
            return false;
        }

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - last_mapping_time_).count();

        // Refresh when 75% of lifetime has elapsed
        if (elapsed < (mapping_lifetime_ * 3 / 4)) {
            return true; // No need to refresh yet
        }

        // Request same mapping with same lifetime
        return map_port(mapped_internal_port_, mapped_external_port_,
                       last_protocol_, mapping_lifetime_);
    }

    void NATPMP::removeMapping() {
        if (mapped_external_port_ != 0) {
            // Request mapping with lifetime = 0 to remove it
            map_port(mapped_internal_port_, mapped_external_port_,
                    last_protocol_, 0);
            mapped_external_port_ = 0;
            mapping_lifetime_ = 0;
        }
    }
}