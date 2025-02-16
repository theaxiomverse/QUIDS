#include "crypto/hybrid/QuantumResistantHash.hpp"
#include "crypto/blake3/Blake3Hash.hpp"
#include "quantum/QuantumCrypto.hpp"
#include <openssl/evp.h>
#include <memory>

namespace quids {
namespace crypto {

class QuantumResistantHash::Impl {
public:
    Impl() : blake3_(), quantum_crypto_() {
        // Initialize OpenSSL
        OpenSSL_add_all_algorithms();
    }

    ~Impl() {
        EVP_cleanup();
    }

    std::vector<uint8_t> hash(const std::vector<uint8_t>& data) {
        // Get BLAKE3 hash
        blake3_.update(data);
        auto blake3_hash = blake3_.finalize();

        // Get quantum hash
        auto quantum_hash = quantum_crypto_.hashData(data);

        // XOR the hashes for hybrid protection
        std::vector<uint8_t> hybrid_hash(64); // 512 bits total
        for (size_t i = 0; i < 32; ++i) {
            hybrid_hash[i] = blake3_hash[i];
            hybrid_hash[i + 32] = quantum_hash[i];
        }

        // Domain separation
        uint8_t domain_tag = 0x01;
        for (auto& byte : hybrid_hash) {
            byte ^= domain_tag;
        }

        // Final BLAKE3 pass for mixing
        Blake3Hash final_hash;
        final_hash.update(hybrid_hash);
        return final_hash.finalize();
    }

private:
    Blake3Hash blake3_;
    quantum::QuantumCrypto quantum_crypto_;
};

QuantumResistantHash::QuantumResistantHash() : impl_(std::make_unique<Impl>()) {}
QuantumResistantHash::~QuantumResistantHash() = default;

std::vector<uint8_t> QuantumResistantHash::hash(const std::vector<uint8_t>& data) {
    return impl_->hash(data);
}

double QuantumResistantHash::getSecurityLevel() const {
    // 256 bits from BLAKE3 + quantum entropy
    return 256.0;
}

std::string QuantumResistantHash::getName() const {
    return "QuantumResistant-BLAKE3-Hybrid";
}

} // namespace crypto
} // namespace quids