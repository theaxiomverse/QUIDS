#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace quids {
namespace crypto {

/**
 * @brief Secure validator authentication using SRP6 protocol
 * 
 * This class implements the SRP6 (Secure Remote Password) protocol for secure
 * validator authentication. SRP6 provides:
 * - Zero-knowledge password proof
 * - Mutual authentication
 * - Perfect forward secrecy
 * - No plaintext passwords transmitted
 */
class SecureValidatorAuth {
public:
    struct Verifier {
        std::vector<uint8_t> salt;
        std::vector<uint8_t> verifier;
    };

    struct ClientSession {
        std::vector<uint8_t> public_ephemeral;
        std::vector<uint8_t> session_key;
        std::vector<uint8_t> proof;
    };

    struct ServerSession {
        std::vector<uint8_t> public_ephemeral;
        std::vector<uint8_t> session_key;
        std::vector<uint8_t> proof;
    };

    SecureValidatorAuth();
    ~SecureValidatorAuth();

    // Delete copy/move operations
    SecureValidatorAuth(const SecureValidatorAuth&) = delete;
    SecureValidatorAuth& operator=(const SecureValidatorAuth&) = delete;
    SecureValidatorAuth(SecureValidatorAuth&&) = delete;
    SecureValidatorAuth& operator=(SecureValidatorAuth&&) = delete;

    /**
     * @brief Generate verifier for a new validator
     * 
     * @param identifier Validator identifier (e.g., node ID)
     * @param password Validator password/secret
     * @return Verifier containing salt and verification value
     */
    Verifier generateVerifier(const std::string& identifier, const std::string& password);

    /**
     * @brief Start client-side authentication (validator)
     * 
     * @param identifier Validator identifier
     * @param password Validator password/secret
     * @return ClientSession containing public ephemeral and proof
     */
    ClientSession startClientAuth(const std::string& identifier, const std::string& password);

    /**
     * @brief Start server-side authentication
     * 
     * @param identifier Validator identifier
     * @param verifier Previously generated verifier
     * @param client_public_ephemeral Client's public ephemeral value
     * @return ServerSession containing server's public ephemeral and proof
     */
    ServerSession startServerAuth(const std::string& identifier,
                                const Verifier& verifier,
                                const std::vector<uint8_t>& client_public_ephemeral);

    /**
     * @brief Verify server's proof on client side
     * 
     * @param server_proof Server's proof value
     * @return true if server proof is valid
     */
    bool verifyServerProof(const std::vector<uint8_t>& server_proof);

    /**
     * @brief Verify client's proof on server side
     * 
     * @param client_proof Client's proof value
     * @return true if client proof is valid
     */
    bool verifyClientProof(const std::vector<uint8_t>& client_proof);

    /**
     * @brief Get the derived session key (if authentication successful)
     * 
     * @return Session key or nullopt if authentication failed
     */
    std::optional<std::vector<uint8_t>> getSessionKey() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace crypto
} // namespace quids 