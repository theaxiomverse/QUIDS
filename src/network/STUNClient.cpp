#include "network/STUNClient.hpp"
#include <cstdint>
#include <array>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <spdlog/spdlog.h>
#include <random>
#include <boost/asio/steady_timer.hpp>
#include <cstring>
#include <chrono>
#include <memory>


namespace quids::network {
    // STUN message header (RFC 5389)
    struct StunHeader {
        uint16_t type;
        uint16_t length;
        uint32_t magic_cookie;
        uint8_t transaction_id[12];
    };

    bool STUNClient::get_mapped_address(const std::string& stun_server, uint16_t port, 
                                      std::string& public_ip, uint16_t& public_port) {
        try {
            boost::asio::io_context io_context;
            boost::asio::ip::udp::socket socket(io_context);
            socket.open(boost::asio::ip::udp::v4());

            // Resolve STUN server address
            boost::asio::ip::udp::resolver resolver(io_context);
            auto endpoints = resolver.resolve(
                boost::asio::ip::udp::v4(),
                stun_server,
                std::to_string(port)
            );

            // Create STUN binding request
            StunHeader request{};
            request.type = htons(0x0001); // Binding Request
            request.length = 0;
            request.magic_cookie = htonl(0x2112A442);
            
            // Generate random transaction ID
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 255);
            for(int i = 0; i < 12; i++) {
                request.transaction_id[i] = static_cast<uint8_t>(dis(gen));
            }

            // Send request
            socket.send_to(boost::asio::buffer(&request, sizeof(request)), *endpoints.begin());

            // Set timeout
            boost::asio::steady_timer timer(socket.get_executor());
            timer.expires_after(std::chrono::seconds(2));

            // Start the timer
            timer.async_wait([&socket](const boost::system::error_code& ec) {
                if (!ec) {
                    // Handle timeout (e.g., log an error, close the socket, etc.)
                    SPDLOG_ERROR("Receive timeout occurred.");
                    socket.close(); // Close the socket or handle as needed
                }
            });

            // Receive response
            std::array<uint8_t, 1024> recv_buffer;
            boost::asio::ip::udp::endpoint sender_endpoint;
            
            size_t len = socket.receive_from(
                boost::asio::buffer(recv_buffer),
                sender_endpoint
            );

            // Cancel the timer if the receive is successful
            timer.cancel();

            if(len > 0) {
                // Parse STUN response
                auto* response = reinterpret_cast<StunHeader*>(recv_buffer.data());
                if(ntohs(response->type) == 0x0101) { // Binding Success Response
                    // Verify transaction ID
                    if(memcmp(request.transaction_id, response->transaction_id, 12) != 0) {
                        SPDLOG_ERROR("STUN transaction ID mismatch");
                        return false;
                    }

                    // Parse attributes
                    size_t offset = sizeof(StunHeader);
                    while(offset + 4 <= len) {
                        uint16_t attr_type = ntohs(*reinterpret_cast<uint16_t*>(recv_buffer.data() + offset));
                        uint16_t attr_length = ntohs(*reinterpret_cast<uint16_t*>(recv_buffer.data() + offset + 2));
                        offset += 4;

                        if(attr_type == 0x0020) { // XOR-MAPPED-ADDRESS
                            if(attr_length >= 8) {
                                uint8_t family = recv_buffer[offset + 1];
                                if(family == 0x01) { // IPv4
                                    // Port is XOR'd with most significant 16 bits of magic cookie
                                    uint16_t xor_port = ntohs(*reinterpret_cast<uint16_t*>(recv_buffer.data() + offset + 2));
                                    public_port = xor_port ^ (request.magic_cookie >> 16);

                                    // Address is XOR'd with magic cookie
                                    uint32_t xor_addr = *reinterpret_cast<uint32_t*>(recv_buffer.data() + offset + 4);
                                    uint32_t addr = ntohl(xor_addr) ^ ntohl(request.magic_cookie);
                                    
                                    boost::asio::ip::address_v4 ip_addr(addr);
                                    public_ip = ip_addr.to_string();
                                    return true;
                                }
                            }
                        }
                        offset += attr_length;
                        // Align to 4-byte boundary
                        offset += (4 - (attr_length % 4)) % 4;
                    }
                }
            }
        } catch (const boost::system::system_error& e) {
            SPDLOG_ERROR("STUN request failed: {}", e.what());
        }

        return false;
    }

    bool STUNClient::detect_symmetric_nat(const std::string& server1,
                                          const std::string& server2) {
        std::string ip1, ip2;
        uint16_t port1, port2;

        // Test with first server
        if (!get_mapped_address(server1, 3478, ip1, port1)) {
            return false;
        }

        // Test with second server
        if (!get_mapped_address(server2, 3478, ip2, port2)) {
            return false;
        }

        // If mapped addresses are different, we're behind a symmetric NAT
        return (ip1 != ip2) || (port1 != port2);
    }

    NATType STUNClient::detect_nat_type(const std::string& stun_server) {
        std::string mapped_ip;
        uint16_t mapped_port;
        
        // Test 1: Basic binding request
        if (!get_mapped_address(stun_server, 3478, mapped_ip, mapped_port)) {
            return NATType::Blocked;
        }

        // Get local address
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
        
        std::string local_ip = socket.local_endpoint().address().to_string();

        // Compare local and mapped addresses
        if (local_ip == mapped_ip) {
            return NATType::Open;
        }

        // Test for symmetric NAT using an alternate server
        std::string alt_server = "stun2.l.google.com"; // Example alternate server
        if (detect_symmetric_nat(stun_server, alt_server)) {
            return NATType::Symmetric;
        }

        // If we got here, it's either Full-cone, Restricted-cone, or Port-restricted
        // Full implementation would require additional tests
        return NATType::RestrictedCone;
    }
}