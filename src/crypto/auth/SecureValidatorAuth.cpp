#include "crypto/auth/SecureValidatorAuth.hpp"
#include <botan/auto_rng.h>
#include <botan/srp6.h>
#include <botan/hash.h>
#include <botan/dl_group.h>

#include <stdexcept>

namespace quids {
namespace crypto {

class SecureValidatorAuth::Impl {
public:
    Impl() : m_hash(Botan::HashFunction::create_or_throw("SHA-256")),
            m_group("modp/srp/2048") {}

    Verifier generateVerifier(const std::string& identifier, const std::string& password) {
        Botan::AutoSeeded_RNG rng;
        
        // Generate random salt
        std::vector<uint8_t> salt(32);
        rng.randomize(salt.data(), salt.size());

        // Create SRP6 verifier
        Botan::BigInt v = Botan::srp6_generate_verifier(identifier,
                                                       password,
                                                       salt,
                                                       m_group,
                                                       "SHA-256");

        // Convert verifier to bytes
        auto verifier_bytes = Botan::BigInt::encode(v);
        
        return Verifier{salt, verifier_bytes};
    }

    ClientSession startClientAuth(const std::string& identifier, const std::string& password) {
        Botan::AutoSeeded_RNG rng;

        // Use Botan's srp6_client_agree function
        auto [A, key] = Botan::srp6_client_agree(identifier, 
                                                password,
                                                m_group,
                                                "SHA-256",
                                                m_salt,  // Use stored salt
                                                m_server_B,
                                                256, // a_bits
                                                rng);

        ClientSession session;
        session.public_ephemeral = Botan::BigInt::encode(A);
        m_session_key = std::vector<uint8_t>(key.begin(), key.end());
        
        return session;
    }

    ServerSession startServerAuth(const std::string& identifier,
                                const Verifier& verifier,
                                const std::vector<uint8_t>& client_public_ephemeral) {
        Botan::AutoSeeded_RNG rng;

        // Store salt for later use
        m_salt = verifier.salt;

        // Convert verifier back to BigInt
        Botan::BigInt v = Botan::BigInt::decode(verifier.verifier);
        
        // Create server session
        m_srp6_server = std::make_unique<Botan::SRP6_Server_Session>();
        
        // Get server's public value B
        m_server_B = m_srp6_server->step1(v, m_group, "SHA-256", 256, rng);

        // Process client's public value for step 2
        auto client_A = Botan::BigInt::decode(client_public_ephemeral);
        auto key = m_srp6_server->step2(client_A);

        ServerSession session;
        session.public_ephemeral = Botan::BigInt::encode(m_server_B);
        m_session_key = std::vector<uint8_t>(key.begin(), key.end());
        
        return session;
    }

    bool verifyServerProof(const std::vector<uint8_t>& server_proof) {
        if (m_session_key.empty()) {
            throw std::runtime_error("No session key available");
        }

        // In SRP6, the server proof is the session key itself
        return server_proof == m_session_key;
    }

    bool verifyClientProof(const std::vector<uint8_t>& client_proof) {
        if (m_session_key.empty()) {
            throw std::runtime_error("No session key available");
        }

        // In SRP6, the client proof is the session key itself
        return client_proof == m_session_key;
    }

    std::optional<std::vector<uint8_t>> getSessionKey() const {
        if (m_session_key.empty()) {
            return std::nullopt;
        }
        return m_session_key;
    }

private:
    std::unique_ptr<Botan::HashFunction> m_hash;
    std::unique_ptr<Botan::SRP6_Server_Session> m_srp6_server;
    std::vector<uint8_t> m_session_key;
    std::vector<uint8_t> m_salt;
    Botan::BigInt m_server_B;
    Botan::DL_Group m_group;
};

SecureValidatorAuth::SecureValidatorAuth() : impl_(std::make_unique<Impl>()) {}
SecureValidatorAuth::~SecureValidatorAuth() = default;

SecureValidatorAuth::Verifier SecureValidatorAuth::generateVerifier(
    const std::string& identifier, const std::string& password) {
    return impl_->generateVerifier(identifier, password);
}

SecureValidatorAuth::ClientSession SecureValidatorAuth::startClientAuth(
    const std::string& identifier, const std::string& password) {
    return impl_->startClientAuth(identifier, password);
}

SecureValidatorAuth::ServerSession SecureValidatorAuth::startServerAuth(
    const std::string& identifier,
    const Verifier& verifier,
    const std::vector<uint8_t>& client_public_ephemeral) {
    return impl_->startServerAuth(identifier, verifier, client_public_ephemeral);
}

bool SecureValidatorAuth::verifyServerProof(const std::vector<uint8_t>& server_proof) {
    return impl_->verifyServerProof(server_proof);
}

bool SecureValidatorAuth::verifyClientProof(const std::vector<uint8_t>& client_proof) {
    return impl_->verifyClientProof(client_proof);
}

std::optional<std::vector<uint8_t>> SecureValidatorAuth::getSessionKey() const {
    return impl_->getSessionKey();
}

} // namespace crypto
} // namespace quids 