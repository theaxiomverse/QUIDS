#pragma once

#include "QuantumForward.hpp"
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace quids::quantum {

class QKDChannel;

class QKDSystem {
public:
    static std::unique_ptr<QKDSystem> create(uint16_t port);
    virtual ~QKDSystem() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    
    virtual std::shared_ptr<QKDChannel> establish_channel(
        const std::string& address, 
        uint16_t port
    ) = 0;
    
    virtual std::optional<std::vector<uint8_t>> receive_message() = 0;

protected:
    QKDSystem() = default;
};

class QKDChannel {
public:
    virtual ~QKDChannel() = default;
    
    virtual std::vector<uint8_t> encrypt(
        const std::vector<uint8_t>& data,
        QuantumSecurityLevel level
    ) = 0;
    
    virtual std::vector<uint8_t> decrypt(
        const std::vector<uint8_t>& data
    ) = 0;

protected:
    QKDChannel() = default;
};

} // namespace quids::quantum 