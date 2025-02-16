#pragma once

#include <memory>
#include <vector>
#include <cstdint>

namespace quids {
namespace crypto {

// Interface class for quantum cryptography integration
class QuantumInterface {
public:
    static QuantumInterface& getInstance();
    ~QuantumInterface();

    // Delete copy/move constructors and assignments
    QuantumInterface(const QuantumInterface&) = delete;
    QuantumInterface& operator=(const QuantumInterface&) = delete;
    QuantumInterface(QuantumInterface&&) = delete;
    QuantumInterface& operator=(QuantumInterface&&) = delete;

    // Initialize the quantum crypto system
    void initialize();

    // Quantum-classical hybrid hash generation
    std::vector<uint8_t> generateHash(const std::vector<uint8_t>& data);

    // Quantum random number generation
    std::vector<uint8_t> generateSeed(size_t size);

private:
    QuantumInterface();
    std::vector<uint8_t> generateClassicalHash(const std::vector<uint8_t>& data);

    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace crypto
} // namespace quids