/**
 * @file QuantumCrypto.hpp
 * @brief Quantum cryptography implementation for secure communication and signatures
 * @author QUIDS Team
 */

#pragma once

#include <vector>
#include <memory>
#include <string>
#include <array>
#include <cstdint>
#include "QKD.hpp"
#include "QuantumState.hpp"
#include "QuantumProof.hpp"


/**
 * @file QuantumCrypto.hpp
 * @brief Quantum cryptography implementation for secure communication and signatures
 * @author QUIDS Team
 */



namespace quids {
namespace quantum {

class QuantumKeyDistribution;

/**
 * @brief Available post-quantum signature schemes
 */
enum class SignatureScheme {
    DILITHIUM,    ///< NIST PQC Round 3 winner for digital signatures
    FALCON,       ///< Alternative NIST PQC signature scheme
    SPHINCS_PLUS  ///< Hash-based signature scheme
};

/**
 * @brief Structure containing quantum key data and properties
 */
struct QuantumKey {
    std::vector<uint8_t> key_material;
    quids::quantum::QuantumState entangled_state{1};  // Initialize with 1 qubit
    double security_parameter{0.0};
    size_t effective_length{0};
};

/**
 * @brief Structure containing quantum signature data and metadata
 */
struct QuantumSignature {
    std::vector<uint8_t> sig_data;  ///< Raw signature data
    SignatureScheme scheme;         ///< Signature scheme used
    double fidelity;               ///< Quantum state fidelity measure
};

/**
 * @brief Parameters for quantum encryption operations
 */
struct QuantumEncryptionParams {
    size_t key_size{256};                              ///< Key size in bits
    bool use_entanglement{true};                       ///< Whether to use quantum entanglement
    double error_rate{0.01};                           ///< Acceptable quantum error rate
    SignatureScheme sig_scheme{SignatureScheme::DILITHIUM}; ///< Default signature scheme
};

/**
 * @brief Main quantum cryptography interface providing encryption and signing capabilities
 */
class QuantumCrypto {
public:
    /**
     * @brief Default constructor
     */
    QuantumCrypto();
    
    /**
     * @brief Virtual destructor
     */
    ~QuantumCrypto();

    // Delete copy/move constructors
    QuantumCrypto(const QuantumCrypto&) = delete;
    QuantumCrypto& operator=(const QuantumCrypto&) = delete;
    QuantumCrypto(QuantumCrypto&&) = delete;
    QuantumCrypto& operator=(QuantumCrypto&&) = delete;
    
    /**
     * @brief Generate cryptographically secure random bytes using quantum entropy
     * @param num_bytes Number of random bytes to generate
     * @return Vector containing the generated random bytes
     */
    std::vector<uint8_t> generateRandomBytes(size_t num_bytes);
    
    /**
     * @brief Generate a quantum-enhanced random seed suitable for PRNGs
     * @return Vector containing the generated seed material
     */
    std::vector<uint8_t> generateSeed();
    
    /**
     * @brief Compute a hybrid quantum-classical hash of input data
     * @param data Input data to be hashed
     * @return Vector containing the computed hash value
     */
    std::vector<uint8_t> hashData(const std::vector<uint8_t>& data);
    
    /**
     * @brief Generate a new quantum cryptographic key
     * @param params Parameters controlling the key generation process
     * @return Generated quantum key structure
     */
        quids::quantum::QuantumKey generateKey(const quids::quantum::QuantumEncryptionParams& params);
    
    /**
     * @brief Create a quantum-enhanced digital signature for given data
     * @param data Data to be signed
     * @param params Parameters controlling the signature generation
     * @return Generated quantum signature structure
     */
    quids::quantum::QuantumSignature sign(const std::vector<uint8_t>& data, 
                         const quids::quantum::QuantumEncryptionParams& params);

    /**
     * @brief Verify a quantum-enhanced digital signature
     * @param data Original data that was signed
     * @param signature Quantum signature to verify
     * @param public_key Public key for verification
     * @return True if signature is valid, false otherwise
     */
    bool verify(const std::vector<uint8_t>& data,
               const quids::quantum::QuantumSignature& signature,
               const quids::quantum::QuantumKey& public_key);
               
    /**
     * @brief Get the current key size in bits
     * @return Key size in bits
     */
    size_t getKeySize() const;
    
    // Security verification
    double measureSecurityLevel(const QuantumKey& key) const;
    bool checkQuantumSecurity(const QuantumState& state) const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    // Internal helper functions
   QuantumState prepareEncryptionState(const std::vector<uint8_t>& data);
   QuantumMeasurement measureEncryptedState(const quids::quantum::QuantumState& state);
    bool validateQuantumParameters(const QuantumEncryptionParams& params) const;
    void updateSecurityMetrics(const quids::quantum::QuantumState& state);
    
    // Constants
    static constexpr size_t MIN_KEY_SIZE = 256;
    static constexpr double MIN_SECURITY_THRESHOLD = 0.99;
    static constexpr size_t MAX_ROUNDS = 1000;


// Helper functions for quantum cryptographic operations
namespace utils {
    // Key management
    QuantumKey deriveQuantumKey(const quids::quantum::QuantumState& state);
    bool validateKeyMaterial(const QuantumKey& key);
    
    // Signature utilities
    quids::quantum::QuantumProof generateSignatureProof(const std::vector<uint8_t>& message,
                                      const quids::quantum::QuantumKey& key);
    double verifySignatureProof(const quids::quantum::QuantumProof& proof,
                              const std::vector<uint8_t>& message);
    
    // Security analysis
    double estimateQuantumSecurity(const quids::quantum::QuantumState& state);
    bool detectQuantumTampering(const quids::quantum::QuantumMeasurement& measurement);
}

}; // namespace quantum
} // namespace quids