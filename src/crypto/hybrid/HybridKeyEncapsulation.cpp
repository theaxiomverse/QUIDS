#include "crypto/hybrid/HybridKeyEncapsulation.hpp"
#include <botan/auto_rng.h>

#include <botan/pk_ops.h>
#include <botan/pubkey.h>
#include <botan/kdf.h>
#include <stdexcept>
#include <algorithm>

// Hybrid Public Key Implementation
Hybrid_PublicKey::Hybrid_PublicKey(std::unique_ptr<Botan::Public_Key> kex, std::unique_ptr<Botan::Public_Key> kem)
    : m_kex_pk(std::move(kex)), m_kem_pk(std::move(kem)) {
    if (!m_kex_pk || !m_kem_pk) {
        throw std::runtime_error("Null arguments not allowed");
    }
}

std::unique_ptr<Botan::PK_Ops::KEM_Encryption> Hybrid_PublicKey::create_kem_encryption_op(std::string_view params, std::string_view provider) const {
    return std::make_unique<Hybrid_Encryption_Operation>(*this, params);
}

std::string Hybrid_PublicKey::algo_name() const {
    return "Hybrid-KEM(" + m_kex_pk->algo_name() + "," + m_kem_pk->algo_name() + ")";
}

const Botan::Public_Key& Hybrid_PublicKey::kex_public_key() const {
    return *m_kex_pk;
}

const Botan::Public_Key& Hybrid_PublicKey::kem_public_key() const {
    return *m_kem_pk;
}

// Hybrid Private Key Implementation
Hybrid_PrivateKey::Hybrid_PrivateKey(std::unique_ptr<Botan::Private_Key> kex, std::unique_ptr<Botan::Private_Key> kem)
    : Hybrid_PublicKey(kex->public_key(), kem->public_key()), m_kex_sk(std::move(kex)), m_kem_sk(std::move(kem)) {}

std::unique_ptr<Botan::PK_Ops::KEM_Decryption> Hybrid_PrivateKey::create_kem_decryption_op(Botan::RandomNumberGenerator& rng, std::string_view params, std::string_view provider) const {
    return std::make_unique<Hybrid_Decryption_Operation>(*this, rng, params);
}

// Hybrid Encryption Operation Implementation
Hybrid_Encryption_Operation::Hybrid_Encryption_Operation(const Hybrid_PublicKey& hybrid_pk, std::string_view kdf)
    : m_hybrid_pk(hybrid_pk), m_kem_encryptor(hybrid_pk.kem_public_key(), "Raw"),
      m_kdf(Botan::KDF::create_or_throw(kdf)) {}

size_t Hybrid_Encryption_Operation::encapsulated_key_length() const {
    return m_hybrid_pk.kex_public_key().raw_public_key_bits().size() + m_kem_encryptor.encapsulated_key_length();
}

size_t Hybrid_Encryption_Operation::shared_key_length(size_t desired_shared_key_length) const {
    return desired_shared_key_length;
}

void Hybrid_Encryption_Operation::kem_encrypt(std::span<uint8_t> out_encapsulated_key,
                                              std::span<uint8_t> out_shared_key,
                                              Botan::RandomNumberGenerator& rng,
                                              size_t desired_shared_key_length,
                                              std::span<const uint8_t> salt) {
    // Generate an ephemeral key pair
    auto ephemeral_keypair = m_hybrid_pk.kex_public_key().generate_another(rng);
    Botan::PK_Key_Agreement kex(*ephemeral_keypair, rng, "Raw");

    // Agree on a shared secret
    const auto kex_shared_key = kex.derive_key(0, m_hybrid_pk.kex_public_key().raw_public_key_bits()).bits_of();
    const auto kex_encapsulated_key = ephemeral_keypair->raw_public_key_bits();

    // Encapsulate a shared secret using the KEM's public key
    auto kem_result = m_kem_encryptor.encrypt(rng);
    const auto& kem_encapsulated_key = kem_result.encapsulated_shared_key();
    const auto& kem_shared_key = kem_result.shared_key();

    // Combine the encapsulated keys
    if (out_encapsulated_key.size() != kex_encapsulated_key.size() + kem_encapsulated_key.size()) {
        throw std::runtime_error("The output span is not the expected size");
    }

    std::copy(kex_encapsulated_key.begin(), kex_encapsulated_key.end(), out_encapsulated_key.begin());
    std::copy(kem_encapsulated_key.begin(), kem_encapsulated_key.end(), out_encapsulated_key.begin() + kex_encapsulated_key.size());

    // Combine the shared keys
    Botan::secure_vector<uint8_t> concat_shared_key;
    concat_shared_key.insert(concat_shared_key.end(), kex_encapsulated_key.begin(), kex_encapsulated_key.end());
    concat_shared_key.insert(concat_shared_key.end(), kex_shared_key.begin(), kex_shared_key.end());
    concat_shared_key.insert(concat_shared_key.end(), kem_encapsulated_key.begin(), kem_encapsulated_key.end());
    concat_shared_key.insert(concat_shared_key.end(), kem_shared_key.begin(), kem_shared_key.end());

    if (out_shared_key.size() < desired_shared_key_length) {
        throw std::runtime_error("The output span is smaller than the requested length");
    }

    m_kdf->derive_key(out_shared_key.first(desired_shared_key_length), concat_shared_key, salt, {});
}

// Hybrid Decryption Operation Implementation
Hybrid_Decryption_Operation::Hybrid_Decryption_Operation(const Hybrid_PrivateKey& hybrid_sk, 
                                                         Botan::RandomNumberGenerator& rng, 
                                                         std::string_view kdf)
    : m_hybrid_sk(hybrid_sk), 
      m_key_agreement(hybrid_sk.kex_private_key(), rng, "Raw"),
      m_kem_decryptor(hybrid_sk.kem_private_key(), rng, "Raw"),
      m_kdf(Botan::KDF::create_or_throw(kdf)) {}

size_t Hybrid_Decryption_Operation::encapsulated_key_length() const {
    return m_hybrid_sk.kex_public_key().raw_public_key_bits().size() + m_kem_decryptor.encapsulated_key_length();
}

size_t Hybrid_Decryption_Operation::shared_key_length(size_t desired_shared_key_length) const {
    return desired_shared_key_length;
}

void Hybrid_Decryption_Operation::kem_decrypt(std::span<uint8_t> out_shared_key,
                                              std::span<const uint8_t> encapsulated_key,
                                              size_t desired_shared_key_length,
                                              std::span<const uint8_t> salt) {
    if (encapsulated_key.size() != encapsulated_key_length()) {
        throw std::runtime_error("The provided encapsulated key is not of the expected length");
    }

    // Extract the ephemeral public key and the KEM's encapsulation
    const auto kex_encapsulated_key = encapsulated_key.subspan(0, m_hybrid_sk.kex_public_key().raw_public_key_bits().size());
    const auto kem_encapsulated_key = encapsulated_key.subspan(kex_encapsulated_key.size());

    // Agree on a shared secret
    const auto kex_shared_key = m_key_agreement.derive_key(0, kex_encapsulated_key).bits_of();

    // Decapsulate a shared secret
    const auto kem_shared_key = m_kem_decryptor.decrypt(kem_encapsulated_key);

    // Combine the shared keys
    Botan::secure_vector<uint8_t> concat_shared_key;
    concat_shared_key.insert(concat_shared_key.end(), kex_encapsulated_key.begin(), kex_encapsulated_key.end());
    concat_shared_key.insert(concat_shared_key.end(), kex_shared_key.begin(), kex_shared_key.end());
    concat_shared_key.insert(concat_shared_key.end(), kem_encapsulated_key.begin(), kem_encapsulated_key.end());
    concat_shared_key.insert(concat_shared_key.end(), kem_shared_key.begin(), kem_shared_key.end());

    if (out_shared_key.size() < desired_shared_key_length) {
        throw std::runtime_error("The output buffer is smaller than the requested key length");
    }

    m_kdf->derive_key(out_shared_key.first(desired_shared_key_length), concat_shared_key, salt, {});
}