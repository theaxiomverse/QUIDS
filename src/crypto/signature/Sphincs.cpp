#include "crypto/signature/Sphincs.hpp"
#include <botan/auto_rng.h>
#include <botan/pubkey.h>
#include <botan/sphincsplus.h>
#include <botan/sp_parameters.h>

namespace quids {
namespace crypto {

class SphincsPlus::Impl {
public:
    Impl() = default;

    void generateKeyPair() {
        Botan::AutoSeeded_RNG rng;
        m_private_key = std::make_unique<Botan::SphincsPlus_PrivateKey>(rng, Botan::Sphincs_Parameter_Set::SLHDSA256Fast, Botan::Sphincs_Hash_Type::Sha256);
        m_public_key = std::unique_ptr<Botan::Public_Key>(m_private_key->public_key().release());
    }

    std::vector<uint8_t> getPublicKey() const {
        if (!m_public_key) {
            throw std::runtime_error("No public key available. Call generateKeyPair() first.");
        }
        return m_public_key->public_key_bits();
    }

    std::vector<uint8_t> getPrivateKey() const {
        if (!m_private_key) {
            throw std::runtime_error("No private key available. Call generateKeyPair() first.");
        }
        auto priv_bits = m_private_key->private_key_bits();
        return std::vector<uint8_t>(priv_bits.begin(), priv_bits.end());
    }

    std::vector<uint8_t> sign(const std::vector<uint8_t>& message) {
        if (!m_private_key) {
            throw std::runtime_error("No private key available. Call generateKeyPair() first.");
        }

        Botan::AutoSeeded_RNG rng;
        Botan::PK_Signer signer(*m_private_key, rng, "SHA-512");
        return signer.sign_message(message, rng);
    }

    bool verify(const std::vector<uint8_t>& message,
               const std::vector<uint8_t>& signature,
               const std::vector<uint8_t>& public_key) {
        try {
            Botan::SphincsPlus_PublicKey verify_key(public_key, Botan::Sphincs_Parameter_Set::SLHDSA256Fast, Botan::Sphincs_Hash_Type::Sha256);
            Botan::PK_Verifier verifier(verify_key, "SHA-512");
            return verifier.verify_message(message, signature);
        } catch (const std::exception& e) {
            return false;
        }
    }

    size_t getSignatureSize() const {
        if (!m_private_key) {
            throw std::runtime_error("No key available. Call generateKeyPair() first.");
        }
        Botan::AutoSeeded_RNG rng;
        Botan::PK_Signer signer(*m_private_key, rng, "SHA-512");
        return signer.signature_length();
    }

    size_t getPublicKeySize() const {
        if (!m_public_key) {
            throw std::runtime_error("No key available. Call generateKeyPair() first.");
        }
        return m_public_key->public_key_bits().size();
    }

    size_t getPrivateKeySize() const {
        if (!m_private_key) {
            throw std::runtime_error("No key available. Call generateKeyPair() first.");
        }
        return m_private_key->private_key_bits().size();
    }

    std::string getName() const {
        return "SPHINCS+";
    }

    int getSecurityLevel() const {
        return 256; // Using 256f variant
    }

    std::string getVariant() const {
        return "SHA2-256f-256";
    }

private:
    std::unique_ptr<Botan::SphincsPlus_PrivateKey> m_private_key;
    std::unique_ptr<Botan::Public_Key> m_public_key;
};

SphincsPlus::SphincsPlus() : impl_(std::make_unique<Impl>()) {}
SphincsPlus::~SphincsPlus() = default;

void SphincsPlus::generateKeyPair() {
    impl_->generateKeyPair();
}

std::vector<uint8_t> SphincsPlus::getPublicKey() const {
    return impl_->getPublicKey();
}

std::vector<uint8_t> SphincsPlus::getPrivateKey() const {
    return impl_->getPrivateKey();
}

std::vector<uint8_t> SphincsPlus::sign(const std::vector<uint8_t>& message) {
    return impl_->sign(message);
}

bool SphincsPlus::verify(const std::vector<uint8_t>& message,
                       const std::vector<uint8_t>& signature,
                       const std::vector<uint8_t>& public_key) {
    return impl_->verify(message, signature, public_key);
}

size_t SphincsPlus::getSignatureSize() const {
    return impl_->getSignatureSize();
}

size_t SphincsPlus::getPublicKeySize() const {
    return impl_->getPublicKeySize();
}

size_t SphincsPlus::getPrivateKeySize() const {
    return impl_->getPrivateKeySize();
}

std::string SphincsPlus::getName() const {
    return impl_->getName();
}

int SphincsPlus::getSecurityLevel() const {
    return impl_->getSecurityLevel();
}

std::string SphincsPlus::getVariant() const {
    return impl_->getVariant();
}

} // namespace crypto
} // namespace quids