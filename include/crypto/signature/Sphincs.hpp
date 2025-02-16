#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <string>

namespace quids {
namespace crypto {

class SphincsPlus {
public:
    SphincsPlus();
    ~SphincsPlus();

    // Delete copy/move
    SphincsPlus(const SphincsPlus&) = delete;
    SphincsPlus& operator=(const SphincsPlus&) = delete;
    SphincsPlus(SphincsPlus&&) = delete;
    SphincsPlus& operator=(SphincsPlus&&) = delete;

    // Key generation
    void generateKeyPair();
    
    // Get public/private keys
    std::vector<uint8_t> getPublicKey() const;
    std::vector<uint8_t> getPrivateKey() const;

    // Sign a message
    std::vector<uint8_t> sign(const std::vector<uint8_t>& message);

    // Verify a signature
    bool verify(const std::vector<uint8_t>& message,
               const std::vector<uint8_t>& signature,
               const std::vector<uint8_t>& public_key);

    // Parameters and info
    size_t getSignatureSize() const;
    size_t getPublicKeySize() const;
    size_t getPrivateKeySize() const;
    std::string getName() const;
    int getSecurityLevel() const;
    std::string getVariant() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace crypto
} // namespace quids