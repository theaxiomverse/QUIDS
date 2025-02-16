#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <string>
#include <botan-3/botan/pubkey.h>

namespace quids {
namespace crypto {

struct KyberKeyPair {
    std::vector<uint8_t> public_key;
    std::vector<uint8_t> private_key;
};

struct KyberCiphertext {
    std::vector<uint8_t> data;
    std::vector<uint8_t> shared_secret;
};

class KyberKEM {
public:
    KyberKEM();
    ~KyberKEM();

    // Delete copy/move
    KyberKEM(const KyberKEM&) = delete;
    KyberKEM& operator=(const KyberKEM&) = delete;
    KyberKEM(KyberKEM&&) = delete;
    KyberKEM& operator=(KyberKEM&&) = delete;

    // Key generation
    KyberKeyPair generateKeyPair();

    // Encapsulation: Generate shared secret and ciphertext
    KyberCiphertext encapsulate(const std::vector<uint8_t>& public_key);

    // Decapsulation: Recover shared secret from ciphertext
    std::vector<uint8_t> decapsulate(const std::vector<uint8_t>& ciphertext,
                                   const std::vector<uint8_t>& private_key);

    // Parameters and info
    size_t getPublicKeySize() const;
    size_t getPrivateKeySize() const;
    size_t getSharedSecretSize() const;
    size_t getCiphertextSize() const;
    std::string getName() const;
    int getSecurityLevel() const;

    // Add audit trail logging
    void logAuditTrail(const std::string& operation, const std::vector<uint8_t>& data);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace crypto
} // namespace quids