#pragma once


#include <botan/pk_ops.h>
#include <botan/pubkey.h>
#include <botan/kdf.h>
#include <memory>
#include <vector>

// Hybrid Public Key Class
class Hybrid_PublicKey : public Botan::Public_Key {
public:
    Hybrid_PublicKey(std::unique_ptr<Botan::Public_Key> kex, std::unique_ptr<Botan::Public_Key> kem);

    std::unique_ptr<Botan::PK_Ops::KEM_Encryption> create_kem_encryption_op(std::string_view params, std::string_view provider) const override;
    std::string algo_name() const override;

    const Botan::Public_Key& kex_public_key() const;
    const Botan::Public_Key& kem_public_key() const;

private:
    std::unique_ptr<Botan::Public_Key> m_kex_pk;
    std::unique_ptr<Botan::Public_Key> m_kem_pk;
};

// Hybrid Private Key Class
class Hybrid_PrivateKey : public virtual Botan::Private_Key, public virtual Hybrid_PublicKey {
public:
    Hybrid_PrivateKey(std::unique_ptr<Botan::Private_Key> kex, std::unique_ptr<Botan::Private_Key> kem);

    std::unique_ptr<Botan::PK_Ops::KEM_Decryption> create_kem_decryption_op(Botan::RandomNumberGenerator& rng, std::string_view params, std::string_view provider) const override;

    // Add this method declaration
     const Botan::Private_Key& kex_private_key() const { return *m_kex_sk; } // Existing method
    const Botan::Private_Key& kem_private_key() const { return *m_kem_sk; } // Add this line


private:
    std::unique_ptr<Botan::Private_Key> m_kex_sk;
    std::unique_ptr<Botan::Private_Key> m_kem_sk;
};

// Hybrid Encryption Operation Class
class Hybrid_Encryption_Operation : public Botan::PK_Ops::KEM_Encryption {
public:
    Hybrid_Encryption_Operation(const Hybrid_PublicKey& hybrid_pk, std::string_view kdf);

    size_t encapsulated_key_length() const override;
    size_t shared_key_length(size_t desired_shared_key_length) const override;
    void kem_encrypt(std::span<uint8_t> out_encapsulated_key, std::span<uint8_t> out_shared_key, Botan::RandomNumberGenerator& rng, size_t desired_shared_key_length, std::span<const uint8_t> salt) override;

private:
    const Hybrid_PublicKey& m_hybrid_pk;
    Botan::PK_KEM_Encryptor m_kem_encryptor;
    std::unique_ptr<Botan::KDF> m_kdf;
};

// Hybrid Decryption Operation Class
class Hybrid_Decryption_Operation : public Botan::PK_Ops::KEM_Decryption {
public:
    Hybrid_Decryption_Operation(const Hybrid_PrivateKey& hybrid_sk, Botan::RandomNumberGenerator& rng, std::string_view kdf);

    size_t encapsulated_key_length() const override;
    size_t shared_key_length(size_t desired_shared_key_length) const override;
    void kem_decrypt(std::span<uint8_t> out_shared_key, std::span<const uint8_t> encapsulated_key, size_t desired_shared_key_length, std::span<const uint8_t> salt) override;

private:
    const Hybrid_PrivateKey& m_hybrid_sk;
    Botan::PK_Key_Agreement m_key_agreement;
    Botan::PK_KEM_Decryptor m_kem_decryptor;
    std::unique_ptr<Botan::KDF> m_kdf;
};