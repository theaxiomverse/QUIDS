#include "network/P2PConnection.hpp"
#include "network/P2PNetwork.hpp"
#include "network/STUNClient.hpp"
#include "network/UPnPClient.hpp"
#include "network/NATPMP.hpp"
#include <memory>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <fmt/format.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <natpmp.h>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <random>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <queue>
#include <optional>
#include <vector>
#include <cstdint>
#include <boost/asio/io_context.hpp>

using namespace fmt::literals;
using namespace spdlog;
using namespace std::chrono_literals; // For ms literal

namespace quids::network {

// Network-related constants
    class NetworkConstants {
    public:
        static constexpr size_t MAX_MESSAGE_SIZE = 1024 * 1024; // 1MB
        static constexpr uint32_t STUN_MAGIC_COOKIE = 0x2112A442;
        static constexpr std::chrono::milliseconds UPNP_DISCOVER_TIMEOUT{2000};
        static constexpr int UPNP_TTL = 2;
        static constexpr int DEFAULT_PORT = 0;
        static constexpr size_t MAX_ADDR_LENGTH = 64;
        static constexpr size_t PORT_STR_LENGTH = 16;
        static constexpr const char* UPNP_PROTOCOL = "UDP";
    };

    // Extracted UPnP functionality into a dedicated class with RAII
    class UPnPHandler {
    public:
        UPnPHandler() = default;
        ~UPnPHandler() = default;

        static bool setupMapping(uint16_t port) {
            try {
                auto devlist = getUPnPDeviceList();
                if (!devlist) return false;

                UPNPUrls urls;
                IGDdatas data;
                std::array<char, NetworkConstants::MAX_ADDR_LENGTH> lanaddr{};
                std::array<char, NetworkConstants::MAX_ADDR_LENGTH> wanaddr{};

                int result = UPNP_GetValidIGD(devlist.get(), &urls, &data,
                                             lanaddr.data(), lanaddr.size(),
                                             wanaddr.data(), wanaddr.size());

                if (result == 1) {
                    std::array<char, NetworkConstants::PORT_STR_LENGTH> port_str{};
                    snprintf(port_str.data(), port_str.size(), "%d", port);

                    result = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,
                                               port_str.data(), port_str.data(),
                                               lanaddr.data(), "QUIDS P2P",
                                               NetworkConstants::UPNP_PROTOCOL, nullptr, "0");
                    FreeUPNPUrls(&urls);
                    return result == 0;
                }
                return false;
            } catch (const std::exception& e) {
                spdlog::error("UPnP setup failed: {}", e.what());
                return false;
            }
        }

        static void removeMapping(uint16_t port) {
            try {
                auto devlist = getUPnPDeviceList();
                if (!devlist) return;

                UPNPUrls urls;
                IGDdatas data;
                std::array<char, NetworkConstants::MAX_ADDR_LENGTH> lanaddr{};
                std::array<char, NetworkConstants::MAX_ADDR_LENGTH> wanaddr{};

                if (UPNP_GetValidIGD(devlist.get(), &urls, &data,
                                     lanaddr.data(), lanaddr.size(),
                                     wanaddr.data(), wanaddr.size()) == 1) {
                    std::array<char, NetworkConstants::PORT_STR_LENGTH> port_str{};
                    snprintf(port_str.data(), port_str.size(), "%d", port);

                    UPNP_DeletePortMapping(urls.controlURL, data.first.servicetype,
                                           port_str.data(), NetworkConstants::UPNP_PROTOCOL, nullptr);
                    FreeUPNPUrls(&urls);
                }
            } catch (const std::exception& e) {
                spdlog::error("Failed to remove UPnP mapping: {}", e.what());
            }
        }

    private:
        static std::unique_ptr<UPNPDev, decltype(&freeUPNPDevlist)> getUPnPDeviceList() {
            UPNPDev* devlist = upnpDiscover(
                    NetworkConstants::UPNP_DISCOVER_TIMEOUT.count(),
                    nullptr, nullptr, 0, 0,
                    NetworkConstants::UPNP_TTL,
                    nullptr
            );
            return {devlist, freeUPNPDevlist};
        }
    };

namespace {
    class NATTraversal {
    public:
        std::optional<bool> setupMapping(uint16_t port) {
            return UPnPHandler::setupMapping(port);
        }

        void removeMapping(uint16_t port) {
            UPnPHandler::removeMapping(port);
        }
    };
}

P2PConnection::~P2PConnection() {
    stop();
}

bool P2PConnection::start() {
    try {
        socket_.open(boost::asio::ip::udp::v4());
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to start P2P connection: {}", e.what());
        return false;
    }
}

void P2PConnection::stop() {
    if (socket_.is_open()) {
        boost::system::error_code ec;
        socket_.close(ec);
    }
    connected_ = false;
}

bool P2PConnection::ping() {
    return connected_;
}

bool P2PConnection::perform_nat_traversal(const std::string& ip, uint16_t port) {
    NATTraversal nat;
    auto result = nat.setupMapping(port);
    return result.value_or(false);
}

void P2PConnection::broadcast(const std::vector<uint8_t>& data) {
    // Implementation
}

bool P2PConnection::send_message(const std::string& peer_address, uint16_t peer_port,
                               const std::vector<uint8_t>& message) {
    return true;
}

void P2PConnection::set_message_handler(std::function<void(const std::string&, uint16_t,
                                                         const std::vector<uint8_t>&)> handler) {
    message_handler_ = std::move(handler);
}

std::vector<PeerInfo> P2PConnection::get_connected_peers() const {
    return {};
}

bool P2PConnection::is_connected() const {
    return connected_;
}

std::string P2PConnection::get_address() const {
    return peer_address_;
}

uint16_t P2PConnection::get_port() const {
    return peer_port_;
}

std::chrono::steady_clock::time_point P2PConnection::get_last_seen() const {
    return last_seen_;
}

void P2PConnection::disconnect() {
    stop();
}

const P2PConnection::ConnectionStats& P2PConnection::get_stats() const {
    return stats_;
}

}