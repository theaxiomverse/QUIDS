#ifndef UPNPCLIENT_HPP
#define UPNPCLIENT_HPP

#include <string>
#include <cstdint>
#include <memory>
#include <boost/asio.hpp> // Include necessary headers

namespace quids::network {

class UPnPClient {
public:
    UPnPClient(); // Constructor
    std::string get_external_ip(); // Declare the method

private:
     class Impl {
    public:
        explicit Impl() : io_context_(), socket_(io_context_) {} // Constructor

        std::string discover_gateway(); // Method declaration

    private:
        boost::asio::io_context io_context_; // Declare io_context_ as a member
        boost::asio::ip::udp::socket socket_; // Declare socket_ as a member
    };

    std::unique_ptr<Impl> impl_; // Declare impl_ as a unique pointer to Impl
};

} // namespace quids::network
#endif // UPNPCLIENT_HPP
