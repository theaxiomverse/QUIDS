#ifndef QUIDS_QUANTUM_QUANTUM_CRYPTO_HPP
#define QUIDS_QUANTUM_QUANTUM_CRYPTO_HPP

/**
 * @file QuantumCrypto.hpp
 * @brief Quantum cryptography implementation for secure communication and signatures
 * @author QUIDS Team
 */

#include "StdNamespace.hpp"
#include "QuantumState.hpp"
#include "QuantumProof.hpp"
#include <vector>
#include <memory>
#include <string>
#include <array>
#include <cstdint>
#include "QKD.hpp"



namespace quids::quantum {

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
    vector<uint8_t> key_material;
    QuantumState entangled_state{1};  // Initialize with 1 qubit
    double security_parameter{0.0};
    size_t effective_length{0};
    size_t key_size{256};                              ///< Key size in bits
    size_t proof_size{256};                              ///< Proof size in bits
    size_t signature_size{256};                          ///< Signature size in bits
    size_t hash_size{256};                              ///< Hash size in bits
    size_t key_material_size{256};      
                ///< Key material size in bits
};

/**
 * @brief Structure containing quantum signature data and metadata
 */
struct QuantumSignature {
        vector<uint8_t> sig_data;  ///< Raw signature data
    SignatureScheme scheme;         ///< Signature scheme used
    double fidelity;               ///< Quantum state 
    double proof_fidelity;         ///< Proof 
    QuantumProof proof;
};

/**
 * @brief Parameters for quantum encryption operations
 */
struct QuantumEncryptionParams {
    size_t key_size{256};                              ///< Key size in bits
    bool use_entanglement{true};                       ///< Whether to use quantum entanglement
    double error_rate{0.01};                           ///< Acceptable quantum error rate
    SignatureScheme sig_scheme{SignatureScheme::DILITHIUM}; ///< Default signature scheme
    [[maybe_unused]] bool use_quantum_hash{true};                            ///< Whether to use quantum hash function
    [[maybe_unused]] bool use_quantum_signature{true};                        ///< Whether to use quantum signature
    [[maybe_unused]] bool use_quantum_key_distribution{true};                  ///< Whether to use quantum key distribution
    [[maybe_unused]] bool use_quantum_random_number_generation{true};         ///< Whether to use quantum random number generation
    [[maybe_unused]] bool use_quantum_entropy{true};                          ///< Whether to use quantum entropy
    [[maybe_unused]] bool use_quantum_random_seed{true};                      ///< Whether to use quantum random seed
    [[maybe_unused]] bool use_quantum_random_bytes{true};                     ///< Whether to use quantum random bytes
    [[maybe_unused]] bool use_quantum_random_seed{true};                      ///< Whether to use quantum random seed
    [[maybe_unused]] bool use_quantum_random_bytes{true};                     ///< Whether to use quantum random bytes
    [[maybe_unused]] double noise_threshold{0.01};                          ///< Noise threshold for quantum cryptography    
    size_t security_parameter{256};                          ///< Security parameter for quantum cryptography   
    size_t key_size{256};                              ///< Key size in bits
    size_t proof_size{256};                              ///< Proof size in bits
    size_t signature_size{256};                          ///< Signature size in bits
    size_t hash_size{256};                              ///< Hash size in bits
    size_t key_material_size{256};                      ///< Key material size in bits
   QuantumKey key;
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
    QuantumCrypto(const QuantumEncryptionParams& params);
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
     * @brief Distribute a quantum key to a recipient
     * @param recipient_id ID of recipient to receive key
     * @param key The quantum key to distribute
     * @return true if distribution succeeded, false otherwise
     */
    bool distributeKey(const std::string& recipient_id, const quids::quantum::QuantumKey& key);
    
    /**
     * @brief Encrypt data using a quantum key
     * @param data Data to be encrypted
     * @param key Quantum key for encryption
     * @return Encrypted data
     */
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data, const quids::quantum::QuantumKey& key);

    /**
     * @brief Generate a quantum key pair
     * @param scheme Signature scheme to use
     * @return Pair of keys (public, private)
     */
    std::pair<std::vector<uint8_t>, std::vector<uint8_t>> generateKeypair(SignatureScheme scheme);

    /**     
     * @brief Decrypt data using a quantum key
     * @param data Data to be decrypted
     * @param key Quantum key for decryption
     * @return Decrypted data
     */
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data, const quids::quantum::QuantumKey& key);

    /**
     * @brief Create a quantum-enhanced digital signature for given data
     * @param data Data to be signed
     * @param params Parameters controlling the signature generation
     * @return Generated quantum signature structure
     */
    quids::quantum::QuantumSignature sign(const std::vector<uint8_t>& data, 
                         const quids::quantum::QuantumEncryptionParams& params);

    /**
     * @brief Sign a message using a private key
     * @param message Message to sign
     * @param private_key Private key to use for signing
     * @return Signed message
     */ 
    quids::quantum::QuantumSignature sign(const std::vector<uint8_t>& message, 
                    const std::vector<uint8_t>& private_key);   

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

    /**
     * @brief Get the current security parameter
     * @return Security parameter
     */
    double getSecurityParameter() const;

    /**
     * @brief Get the current effective key length
     * @return Effective key length
     */
    size_t getEffectiveKeyLength() const;

    /**
     * @brief Get the current quantum state
     * @return Quantum state
     */
    quids::quantum::QuantumState getQuantumState() const;
    

    /**
     * @brief Get the current quantum proof
     * @return Quantum proof
     */
    quids::quantum::QuantumProof getQuantumProof() const;
    
    
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
}; // End of QuantumCrypto class

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
} // namespace utils

    } // namespace quids::quantum


#endif // QUIDS_QUANTUM_QUANTUM_CRYPTO_HPP
