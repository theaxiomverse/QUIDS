#include "crypto/signature/Dilithium.hpp"
#include <botan/auto_rng.h>
#include <botan/pubkey.h>
#include <botan/dilithium.h>

namespace quids {
namespace crypto {

class DilithiumSigner::Impl {
public:
    Impl() = default;

    void generateKeyPair() {
        Botan::AutoSeeded_RNG rng;
        m_private_key = std::make_unique<Botan::Dilithium_PrivateKey>(rng, Botan::DilithiumMode(Botan::DilithiumMode::Dilithium8x7));
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
        Botan::PK_Signer signer(*m_private_key, rng, "Randomized");
        return signer.sign_message(message, rng);
    }

    bool verify(const std::vector<uint8_t>& message,
               const std::vector<uint8_t>& signature,
               const std::vector<uint8_t>& public_key) {
        try {
            Botan::Dilithium_PublicKey verify_key(public_key, Botan::DilithiumMode(Botan::DilithiumMode::Dilithium8x7));
            Botan::PK_Verifier verifier(verify_key, "Randomized");
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
        Botan::PK_Signer signer(*m_private_key, rng, "Randomized");
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
        return "DILITHIUM8x7";
    }

private:
    std::unique_ptr<Botan::Dilithium_PrivateKey> m_private_key;
    std::unique_ptr<Botan::Public_Key> m_public_key;
};

DilithiumSigner::DilithiumSigner() : impl_(std::make_unique<Impl>()) {}
DilithiumSigner::~DilithiumSigner() = default;

void DilithiumSigner::generateKeyPair() {
    impl_->generateKeyPair();
}

std::vector<uint8_t> DilithiumSigner::getPublicKey() const {
    return impl_->getPublicKey();
}

std::vector<uint8_t> DilithiumSigner::getPrivateKey() const {
    return impl_->getPrivateKey();
}

std::vector<uint8_t> DilithiumSigner::sign(const std::vector<uint8_t>& message) {
    return impl_->sign(message);
}

bool DilithiumSigner::verify(const std::vector<uint8_t>& message,
                           const std::vector<uint8_t>& signature,
                           const std::vector<uint8_t>& public_key) {
    return impl_->verify(message, signature, public_key);
}

size_t DilithiumSigner::getSignatureSize() const {
    return impl_->getSignatureSize();
}

size_t DilithiumSigner::getPublicKeySize() const {
    return impl_->getPublicKeySize();
}

size_t DilithiumSigner::getPrivateKeySize() const {
    return impl_->getPrivateKeySize();
}

std::string DilithiumSigner::getName() const {
    return impl_->getName();
}

} // namespace crypto
} // namespace quids