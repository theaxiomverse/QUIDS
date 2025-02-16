#include "crypto/kyber/BotanKyber.hpp"
#include <botan/auto_rng.h>
#include <botan/pk_keys.h>
#include <botan/pubkey.h>
#include <botan/kyber.h>
#include <stdexcept>

namespace quids {
namespace crypto {

class KyberKEM::Impl {
public:
    Impl() = default;

    KyberKeyPair generateKeyPair() {
        Botan::AutoSeeded_RNG rng;
        Botan::Kyber_PrivateKey private_key(rng, Botan::KyberMode(Botan::KyberMode::Kyber1024_R3));
        std::unique_ptr<Botan::Public_Key> public_key = private_key.public_key();

        KyberKeyPair keypair;
        auto pub_bits = public_key->public_key_bits();
        auto priv_bits = private_key.private_key_bits();
        
        keypair.public_key = std::vector<uint8_t>(pub_bits.begin(), pub_bits.end());
        keypair.private_key = std::vector<uint8_t>(priv_bits.begin(), priv_bits.end());

        return keypair;
    }

    KyberCiphertext encapsulate(const std::vector<uint8_t>& public_key) {
        Botan::AutoSeeded_RNG rng;
        Botan::Kyber_PublicKey kyber_public(public_key, Botan::KyberMode(Botan::KyberMode::Kyber1024_R3));
        
        KyberCiphertext result;
        Botan::PK_KEM_Encryptor encryptor(kyber_public, rng, "KDF2(SHA-256)");
        
        auto enc_result = encryptor.encrypt(rng, 32); // Generate 256-bit shared secret
        result.data = enc_result.encapsulated_shared_key();
        result.shared_secret = std::vector<uint8_t>(enc_result.shared_key().begin(), enc_result.shared_key().end());
        
        return result;
    }

    std::vector<uint8_t> decapsulate(const std::vector<uint8_t>& ciphertext,
                                   const std::vector<uint8_t>& private_key) {
        Botan::AutoSeeded_RNG rng;
        Botan::Kyber_PrivateKey kyber_private(private_key, Botan::KyberMode(Botan::KyberMode::Kyber1024_R3));
        
        Botan::PK_KEM_Decryptor decryptor(kyber_private, rng, "KDF2(SHA-256)");
        auto dec_result = decryptor.decrypt(ciphertext, 32);
        return std::vector<uint8_t>(dec_result.begin(), dec_result.end());
    }

    size_t getPublicKeySize() const {
        Botan::AutoSeeded_RNG rng;
        Botan::Kyber_PrivateKey temp(rng, Botan::KyberMode(Botan::KyberMode::Kyber1024_R3));
        auto pub = temp.public_key();
        return pub->public_key_bits().size();
    }

    size_t getPrivateKeySize() const {
        Botan::AutoSeeded_RNG rng;
        Botan::Kyber_PrivateKey temp(rng, Botan::KyberMode(Botan::KyberMode::Kyber1024_R3));
        return temp.private_key_bits().size();
    }

    size_t getSharedSecretSize() const {
        return 32; // 256 bits
    }

    size_t getCiphertextSize() const {
        Botan::AutoSeeded_RNG rng;
        Botan::Kyber_PrivateKey temp(rng, Botan::KyberMode(Botan::KyberMode::Kyber1024_R3));
        auto pub = temp.public_key();
        Botan::PK_KEM_Encryptor encryptor(*pub, rng, "KDF2(SHA-256)");
        return encryptor.encapsulated_key_length();
    }
};

KyberKEM::KyberKEM() : impl_(std::make_unique<Impl>()) {}
KyberKEM::~KyberKEM() = default;

KyberKeyPair KyberKEM::generateKeyPair() {
    return impl_->generateKeyPair();
}

KyberCiphertext KyberKEM::encapsulate(const std::vector<uint8_t>& public_key) {
    return impl_->encapsulate(public_key);
}

std::vector<uint8_t> KyberKEM::decapsulate(const std::vector<uint8_t>& ciphertext,
                                         const std::vector<uint8_t>& private_key) {
    return impl_->decapsulate(ciphertext, private_key);
}

size_t KyberKEM::getPublicKeySize() const {
    return impl_->getPublicKeySize();
}

size_t KyberKEM::getPrivateKeySize() const {
    return impl_->getPrivateKeySize();
}

size_t KyberKEM::getSharedSecretSize() const {
    return impl_->getSharedSecretSize();
}

size_t KyberKEM::getCiphertextSize() const {
    return impl_->getCiphertextSize();
}

std::string KyberKEM::getName() const {
    return "KYBER768";
}

int KyberKEM::getSecurityLevel() const {
    return 192; // KYBER768 provides 192-bit security
}

} // namespace crypto
} // namespace quids