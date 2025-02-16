#pragma once

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <chrono>

namespace quids::network {

struct PeerInfo {
    std::string id;
    std::string address;
    uint16_t port;
    std::chrono::steady_clock::time_point last_seen;
};

struct Config {
    uint16_t port{0};
    std::string stun_server;
    uint16_t stun_port{3478};
    bool enable_upnp{true};
    bool enable_nat_pmp{true};
    size_t max_peers{10};
    std::chrono::milliseconds hole_punch_timeout{1000};
    std::chrono::milliseconds keep_alive_interval{5000};
};

class SocketManager {
private:
    boost::asio::ip::udp::socket socket_;
    boost::asio::io_context& io_context_;
    uint16_t port_;

public:
    SocketManager(boost::asio::io_context& io_context, uint16_t port)
        : socket_(io_context), io_context_(io_context), port_(port) {}
    
    ~SocketManager() = default;
};

class P2PConnection {
public:
    explicit P2PConnection(boost::asio::io_context& io_context, const Config& config)
        : io_context_(io_context),
          socket_(io_context),
          config_(config),
          connected_(false),
          peer_port_(0),
          last_seen_(std::chrono::steady_clock::now()) {
        stats_.connected_since = std::chrono::steady_clock::now();
    }
    ~P2PConnection() = default;

    bool start();
    void stop();
    bool ping();
    bool perform_nat_traversal(const std::string& ip, uint16_t port);
    void broadcast(const std::vector<uint8_t>& data);
    bool send_message(const std::string& peer_address, uint16_t peer_port,
                     const std::vector<uint8_t>& message);
    void set_message_handler(std::function<void(const std::string&, uint16_t,
                                              const std::vector<uint8_t>&)> handler);
    std::vector<PeerInfo> get_connected_peers() const;

    bool is_connected() const;
    std::string get_address() const;
    uint16_t get_port() const;
    std::chrono::steady_clock::time_point get_last_seen() const;
    void disconnect();
    struct ConnectionStats {
        size_t bytes_sent{0};
        size_t bytes_received{0};
        size_t messages_sent{0};
        size_t messages_received{0};
        std::chrono::steady_clock::time_point connected_since;
    };
    const ConnectionStats& get_stats() const;

private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::udp::socket socket_;
    const Config config_;
    std::function<void(const std::string&, uint16_t,
                      const std::vector<uint8_t>&)> message_handler_;
    bool connected_{false};
    std::string peer_address_;
    uint16_t peer_port_{0};
    std::chrono::steady_clock::time_point last_seen_;
    ConnectionStats stats_;
};

} // namespace quids::network