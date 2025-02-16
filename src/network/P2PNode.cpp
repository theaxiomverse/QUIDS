#include "network/P2PNode.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <cstring>

using namespace std::chrono_literals;

namespace quids::network {

// Implementation of P2PNode methods using the Impl struct defined in the header
P2PNode::P2PNode(const Config& config)
    : config_(config), impl_(std::make_unique<Impl>()) {
    impl_->config = config;
    impl_->socket_ = boost::asio::ip::udp::socket(impl_->io_context_);
    impl_->accept_thread = std::thread();
    impl_->management_thread = std::thread();
    impl_->message_thread = std::thread();
    impl_->should_stop = false;
    impl_->connections.clear();
    // Initialize other members...
}

P2PNode::~P2PNode() {
    stop();
    cleanup();
}

bool P2PNode::start() {
    if (impl_->should_stop) return false;

    try {
        // Open and bind socket
        impl_->socket_.open(boost::asio::ip::udp::v4());
        boost::asio::ip::udp::endpoint endpoint(
            boost::asio::ip::make_address(config_.bind_address.empty() ? "0.0.0.0" : config_.bind_address),
            config_.bind_port
        );
        impl_->socket_.bind(endpoint);

        // Start threads
        impl_->accept_thread = std::thread([this]() { accept_connections(); });
        impl_->management_thread = std::thread([this]() { manage_connections(); });
        impl_->message_thread = std::thread([this]() { handle_incoming_messages(); });

        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to start P2P node: {}", e.what());
        return false;
    }
}

void P2PNode::stop() {
    impl_->should_stop = true;
    
    if (impl_->socket_.is_open()) {
        boost::system::error_code ec;
        impl_->socket_.close(ec);
    }

    if (impl_->accept_thread.joinable()) impl_->accept_thread.join();
    if (impl_->management_thread.joinable()) impl_->management_thread.join();
    if (impl_->message_thread.joinable()) impl_->message_thread.join();
}

bool P2PNode::connect_to_peer(const std::string& address, uint16_t port) {
    if (impl_->should_stop) return false;

    try {
        Config conn_config;
        conn_config.port = port;
        conn_config.stun_server = config_.stun_server;
        conn_config.stun_port = config_.stun_port;
        conn_config.enable_upnp = config_.enable_upnp;
        conn_config.enable_nat_pmp = config_.enable_nat_pmp;

        auto connection = std::make_shared<P2PConnection>(impl_->io_context_, conn_config);
        connection->set_message_handler(impl_->message_handler_);

        if (connection->start() && connection->perform_nat_traversal(address, port)) {
            std::lock_guard<std::mutex> lock(impl_->connections_mutex);
            impl_->connections[address + ":" + std::to_string(port)] = connection;
            return true;
        }
    } catch (const std::exception& e) {
        spdlog::error("Failed to connect to peer {}:{}: {}", address, port, e.what());
    }
    return false;
}

std::vector<P2PNode::PeerInfo> P2PNode::get_connected_peers() const {
    std::vector<PeerInfo> peers;
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);

    for (const auto& [_, connection] : impl_->connections) {
        if (connection->is_connected()) {
            PeerInfo info;
            info.address = connection->get_address();
            info.port = connection->get_port();
            info.last_seen = std::chrono::system_clock::now();
            const auto& stats = connection->get_stats();
            info.messages_sent = stats.messages_sent;
            info.messages_received = stats.messages_received;
            info.is_connected = true;
            peers.push_back(info);
        }
    }
    return peers;
}

bool P2PNode::broadcast_message(const std::vector<uint8_t>& message) {
    if (impl_->should_stop) return false;

    bool success = true;
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);

    for (const auto& [_, connection] : impl_->connections) {
        if (connection->is_connected()) {
            success &= connection->send_message(connection->get_address(), 
                                             connection->get_port(), 
                                             message);
        }
    }
    return success;
}

void P2PNode::cleanup() noexcept {
    try {
        std::lock_guard<std::mutex> lock(impl_->connections_mutex);
        impl_->connections.clear();
    } catch (...) {
        // Ignore any exceptions during cleanup
    }
}

void P2PNode::accept_connections() {
    std::vector<uint8_t> buffer(1024); // Fixed buffer size
    boost::asio::ip::udp::endpoint sender_endpoint;

    while (!impl_->should_stop) {
        try {
            // Receive incoming UDP packets
            size_t bytes_received = impl_->socket_.receive_from(
                boost::asio::buffer(buffer), sender_endpoint);

            if (bytes_received > 0) {
                std::string client_ip = sender_endpoint.address().to_string();
                uint16_t client_port = sender_endpoint.port();

                // Create connection config
                Config conn_config;
                conn_config.port = config_.port;
                conn_config.stun_server = config_.stun_server;
                conn_config.stun_port = config_.stun_port;
                conn_config.enable_upnp = config_.enable_upnp;
                conn_config.enable_nat_pmp = config_.enable_nat_pmp;
                conn_config.max_peers = config_.max_peers;

                // Create and start connection
                auto connection = std::make_shared<P2PConnection>(impl_->io_context_, conn_config);
                if (connection->start()) {
                    std::string peer_key = client_ip + ":" + std::to_string(client_port);
                    {
                        std::lock_guard<std::mutex> lock(impl_->connections_mutex);
                        impl_->connections[peer_key] = connection;
                    }
                    
                    update_peer_info(client_ip, client_port, true);
                }
            }
        } catch (const std::exception& e) {
            spdlog::error("Error accepting connection: {}", e.what());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void P2PNode::manage_connections() {
    while (!impl_->should_stop) {
        cleanup_disconnected_peers();
        
        // Check connection health
        std::lock_guard<std::mutex> lock(impl_->connections_mutex);
        for (auto& [key, connection] : impl_->connections) {
            if (connection->is_connected()) {
                auto last_seen = connection->get_last_seen();
                auto now = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - last_seen).count();
                
                if (duration > static_cast<int64_t>(config_.connection_timeout_ms)) {
                    connection->disconnect();
                } else if (duration > static_cast<int64_t>(config_.ping_interval_ms)) {
                    connection->ping();
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void P2PNode::handle_incoming_messages() {
    std::vector<uint8_t> buffer(1024);
    boost::asio::ip::udp::endpoint sender_endpoint;

    while (!impl_->should_stop) {
        try {
            size_t bytes_received = impl_->socket_.receive_from(
                boost::asio::buffer(buffer), sender_endpoint);

            if (bytes_received > 0 && impl_->message_handler_) {
                impl_->message_handler_(
                    sender_endpoint.address().to_string(),
                    sender_endpoint.port(),
                    std::vector<uint8_t>(buffer.begin(), buffer.begin() + bytes_received)
                );
            }
        } catch (const std::exception& e) {
            spdlog::error("Error receiving message: {}", e.what());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void P2PNode::cleanup_disconnected_peers() {
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);
    for (auto it = impl_->connections.begin(); it != impl_->connections.end();) {
        if (!it->second->is_connected()) {
            update_peer_info(it->second->get_address(), it->second->get_port(), false);
            it = impl_->connections.erase(it);
        } else {
            ++it;
        }
    }
}

bool P2PNode::validate_peer(const std::string& address, uint16_t port) {
    if (address.empty() || port == 0) {
        return false;
    }

    std::lock_guard<std::mutex> lock(impl_->connections_mutex);
    return impl_->connections.size() < config_.max_connections;
}

void P2PNode::update_peer_info(const std::string& address, uint16_t port, bool connected) {
    // This method can be extended to maintain persistent peer information
    // For now, it just logs connection status changes
    std::cout << "Peer " << address << ":" << port 
              << (connected ? " connected" : " disconnected") << std::endl;
}

void P2PNode::process_incoming_connections() {
    auto buffer = std::make_shared<std::vector<uint8_t>>(1024); // Use fixed size
    auto sender = std::make_shared<boost::asio::ip::udp::endpoint>();

    impl_->socket_.async_receive_from(
        boost::asio::buffer(*buffer), *sender,
        [this, buffer, sender](const boost::system::error_code& error, size_t len) {
            if (!error) {
                // Handle NAT traversal first
                if (len >= 4 && std::equal(buffer->begin(), buffer->begin() + 4, "PUNCH")) {
                    handle_nat_traversal_response(error, len);
                } else if (impl_->message_handler_) {
                    impl_->message_handler_(
                        sender->address().to_string(),
                        sender->port(),
                        std::vector<uint8_t>(buffer->begin(), buffer->begin() + len)
                    );
                }
            }
            process_incoming_connections(); // Continue listening
        }
    );
}

void P2PNode::handle_nat_traversal_response(
    const boost::system::error_code& error, 
    size_t bytes_transferred
) {
    (void)error; // Explicitly mark as unused
    (void)bytes_transferred;
    
    // Implementation using the parameters
    if (!error && bytes_transferred > 0) {
        // Actual handling code
    }
}

} // namespace quids::network 