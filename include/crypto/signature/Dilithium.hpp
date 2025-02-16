#pragma once

#include <vector>
#include <memory>
#include <cstdint>

namespace quids {
namespace crypto {

class DilithiumSigner {
public:
    DilithiumSigner();
    ~DilithiumSigner();

    // Delete copy/move
    DilithiumSigner(const DilithiumSigner&) = delete;
    DilithiumSigner& operator=(const DilithiumSigner&) = delete;
    DilithiumSigner(DilithiumSigner&&) = delete;
    DilithiumSigner& operator=(DilithiumSigner&&) = delete;

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

    // Get parameters and info
    size_t getSignatureSize() const;
    size_t getPublicKeySize() const;
    size_t getPrivateKeySize() const;
    std::string getName() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace crypto
} // namespace quids