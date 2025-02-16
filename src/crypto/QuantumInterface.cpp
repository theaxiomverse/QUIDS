#include "crypto/QuantumHashFunction.hpp"
#include "quantum/QuantumCrypto.hpp"
#include "quantum/QuantumTypes.hpp"
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <memory>
#include <stdexcept>

namespace quids {
namespace crypto {

class QuantumInterface {
public:
    static QuantumInterface& getInstance() {
        static QuantumInterface instance;
        return instance;
    }

    // Initialize quantum crypto system
    void initialize() {
        quantum_crypto_ = std::make_unique<quantum::QuantumCrypto>();
        OpenSSL_add_all_algorithms();
    }

    // Cleanup
    ~QuantumInterface() {
        EVP_cleanup();
    }

    // Generate quantum-safe hash
    std::vector<uint8_t> generateHash(const std::vector<uint8_t>& data) {
        // Combine quantum and classical hashing
        std::vector<uint8_t> quantum_hash = quantum_crypto_->hashData(data);
        std::vector<uint8_t> classical_hash = generateClassicalHash(data);

        // XOR the hashes
        std::vector<uint8_t> combined_hash(32);
        for (size_t i = 0; i < 32; ++i) {
            combined_hash[i] = quantum_hash[i] ^ classical_hash[i];
        }

        return combined_hash;
    }

    // Generate random seed using quantum RNG
    std::vector<uint8_t> generateSeed(size_t size) {
        return quantum_crypto_->generateRandomBytes(size);
    }

private:
    QuantumInterface() {
        initialize();
    }

    // Classical SHA-256 hash
    std::vector<uint8_t> generateClassicalHash(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> hash(SHA256_DIGEST_LENGTH);
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, data.data(), data.size());
        SHA256_Final(hash.data(), &sha256);
        return hash;
    }

    std::unique_ptr<quantum::QuantumCrypto> quantum_crypto_;
};

} // namespace crypto
} // namespace quids