#pragma once
#include <string>
#include <cstdint>

namespace quids::network {

enum class NATType {
    Open,
    FullCone,
    RestrictedCone,
    PortRestrictedCone,
    Symmetric,
    Blocked
};

class STUNClient {
public:
    static bool get_mapped_address(const std::string& stun_server, uint16_t port,
                                 std::string& public_ip, uint16_t& public_port);
        
    static bool detect_symmetric_nat(const std::string& server1, 
                                   const std::string& server2);

    static NATType detect_nat_type(const std::string& stun_server);
};

} 