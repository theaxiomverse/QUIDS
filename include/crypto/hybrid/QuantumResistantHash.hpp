#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <memory>

namespace quids {
namespace crypto {

class QuantumResistantHash {
public:
    QuantumResistantHash();
    ~QuantumResistantHash();

    // Delete copy/move
    QuantumResistantHash(const QuantumResistantHash&) = delete;
    QuantumResistantHash& operator=(const QuantumResistantHash&) = delete;
    QuantumResistantHash(QuantumResistantHash&&) = delete;
    QuantumResistantHash& operator=(QuantumResistantHash&&) = delete;

    // Primary hash function - returns 256-bit hash
    std::vector<uint8_t> hash(const std::vector<uint8_t>& data);

    // Get estimated security level in bits
    double getSecurityLevel() const;

    // Get algorithm name
    std::string getName() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace crypto
} // namespace quids