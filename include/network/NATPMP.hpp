#ifndef NATPMP_HPP
#define NATPMP_HPP

#include <boost/asio.hpp>
#include <chrono>
#include <optional>
#include <iostream>

namespace quids::network {

enum class Protocol {
    TCP,
    UDP
};

class NATPMP {
public:
    NATPMP(boost::asio::io_context& io_context);

     // Method to discover the gateway
    std::string discover_gateway(); // Add this line
    
    // Method to set up a mapping
    std::optional<bool> setupMapping(uint16_t internal_port, uint16_t external_port, uint16_t lifetime);

        // Method to set up a mapping
    bool map_port(uint16_t internal_port, uint16_t external_port, Protocol proto, uint32_t lifetime); // Add this line

    // Method to refresh a mapping
    bool refreshMapping();
    // Method to remove a mapping
    void removeMapping();

    // Method to get the last seen time
    std::chrono::system_clock::time_point get_last_seen() const;

private:
    // Member variables
    uint16_t mapped_internal_port_;
    uint16_t mapped_external_port_;
    uint16_t mapping_lifetime_;
    boost::asio::ip::udp::socket socket_;
    std::chrono::system_clock::time_point last_seen_;
     std::chrono::steady_clock::time_point last_mapping_time_; // Add this line
     Protocol last_protocol_;
     boost::asio::io_context& io_context_;
     boost::asio::steady_timer timer_;
     std::string gateway_;
   
     boost::asio::ip::udp::resolver resolver_;
     
  

};

} // namespace quids::network

#endif // NATPMP_HPP