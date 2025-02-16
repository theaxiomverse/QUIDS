#ifndef QUIDS_CONSENSUS_OPTIMIZED_POBPC_HPP
#define QUIDS_CONSENSUS_OPTIMIZED_POBPC_HPP

#include "StdNamespace.hpp"
#include "quantum/QuantumTypes.hpp"
#include "quantum/QuantumState.hpp"
#include "quantum/QuantumCircuit.hpp"
#include "quantum/QuantumCrypto.hpp"
#include "utils/LockFreeQueue.hpp"

namespace utils {
    template<typename T>
    class LockFreeQueue;
}

/**
 * @file OptimizedPOBPC.hpp
 * @brief Optimized Proof of Batch Probabilistic Consensus implementation
 * 
 * This file implements the POBPC consensus mechanism, which combines batch processing,
 * probabilistic witness selection, and quantum-enhanced security features. The consensus
 * mechanism is designed for high throughput and security in distributed systems.
 * 
 * Key features:
 * - Batch transaction processing with SIMD optimization
 * - Quantum-enhanced zero-knowledge proofs
 * - Probabilistic witness selection
 * - Byzantine fault tolerance
 * - Adaptive consensus parameters
 * 
 * @see proof_of_batch_probabilistic_consensus.md for detailed specification
 */

namespace quids::consensus {

// Common type declarations from std namespace
using std::size_t;
using std::uint64_t;
using std::string;
using std::vector;
using std::atomic;
using std::array;
using std::unique_ptr;
using std::chrono::milliseconds;
using std::min;
using std::max;

/**
 * @brief Configuration for batch processing in POBPC
 * 
 * This structure contains all configuration parameters for the POBPC consensus
 * mechanism, including batch sizes, witness counts, timeouts, and quantum
 * circuit parameters.
 */
struct BatchConfig {
    size_t max_transactions{1000};          ///< Maximum transactions per batch
    size_t witness_count{4};                ///< Number of witnesses per batch
    double consensus_threshold{0.75};        ///< Required consensus threshold (2/3 by default)
    bool use_quantum_proofs{true};          ///< Whether to use quantum-enhanced ZKP
    size_t batch_size{256};                 ///< Size of transaction batches
    size_t num_parallel_verifiers{8};       ///< Number of parallel verifiers
    size_t quantum_circuit_depth{32};       ///< Depth of quantum circuits for ZKP
    bool enable_error_correction{true};      ///< Whether to use quantum error correction
    
    // Additional POBPC-specific parameters
    milliseconds batch_timeout{1000};        ///< Max time to wait for batch formation
    double witness_selection_entropy{1.0};   ///< Entropy factor for witness selection
    size_t min_witness_reliability{80};      ///< Minimum witness reliability score (%)
    size_t max_batch_verification_time{500}; ///< Maximum time for batch verification (ms)
    bool adaptive_witness_selection{true};   ///< Enable adaptive witness count based on load
    size_t recursive_zkp_layers{2};         ///< Number of recursive ZKP layers for critical txs
    
    /**
     * @brief Validates the configuration
     * 
     * Checks if all configuration parameters are within valid ranges and
     * satisfy the requirements for Byzantine fault tolerance and quantum
     * circuit limitations.
     * 
     * @return true if configuration is valid
     */
    [[nodiscard]] bool isValid() const noexcept {
        return max_transactions > 0 &&
               witness_count >= 3 &&  // Minimum for Byzantine fault tolerance
               consensus_threshold >= 0.66 && // At least 2/3 for BFT
               consensus_threshold <= 1.0 &&
               batch_size > 0 &&
               batch_size <= max_transactions &&
               num_parallel_verifiers > 0 &&
               quantum_circuit_depth > 0 &&
               quantum_circuit_depth <= 100 && // Practical limit for current quantum systems
               min_witness_reliability > 50 && // Majority reliability required
               min_witness_reliability <= 100 &&
               recursive_zkp_layers > 0 &&
               recursive_zkp_layers <= 5; // Practical limit for recursive ZKP
    }
};

/**
 * @brief Optimized Proof-of-batch-probabilistic Consensus
 * 
 * This class implements an optimized version of POBPC that combines
 * classical Byzantine consensus with quantum-enhanced security features.
 * It uses SIMD operations for batch processing and quantum circuits for
 * enhanced security verification.
 */
class OptimizedPOBPC {
public:
    /**
     * @brief Information about a consensus witness
     * 
     * This structure maintains information about a witness node in the POBPC
     * consensus, including its identity, reliability metrics, and quantum state
     * for verification purposes.
     */
    struct WitnessInfo {
        string node_id;                                ///< Unique node identifier
        vector<uint8_t> public_key;                   ///< Witness public key
        atomic<double> reliability_score{1.0};        ///< Reliability metric (0-1)
        atomic<uint64_t> last_active{0};             ///< Last active timestamp
        quantum::QuantumState quantum_state;          ///< Quantum state for verification
        atomic<size_t> successful_validations{0};     ///< Successful validations count
        atomic<size_t> total_validations{0};         ///< Total validations attempted

        /**
         * @brief Calculates the current reliability ratio
         * @return Reliability score between 0 and 1
         */
        [[nodiscard]] double calculateReliability() const noexcept {
            const size_t total = total_validations.load();
            return total > 0 ? 
                static_cast<double>(successful_validations.load()) / total : 0.0;
        }

        /**
         * @brief Updates witness reliability based on validation result
         * @param success Whether the validation was successful
         */
        void updateReliability(bool success) noexcept {
            if (success) {
                successful_validations++;
            }
            total_validations++;
            reliability_score.store(calculateReliability());
        }

        /**
         * @brief Checks if witness is currently active
         * @param timeout_ms Timeout in milliseconds
         * @return true if witness is active
         */
        [[nodiscard]] bool isActive(uint64_t timeout_ms) const noexcept {
            const auto now = std::chrono::system_clock::now().time_since_epoch().count();
            return (now - last_active.load()) < timeout_ms;
        }
    };

    /**
     * @brief Proof of batch consensus
     * 
     * This structure contains all data related to a batch proof in the POBPC
     * consensus, including transaction data, witness signatures, ZKP data,
     * and quantum verification state.
     */
    struct BatchProof {
        uint64_t timestamp{0};                            ///< Proof timestamp
        size_t transaction_count{0};                      ///< Number of transactions
        vector<uint8_t> batch_hash;                       ///< Hash of batch contents
        vector<uint8_t> proof_data;                       ///< Classical ZKP data
        vector<vector<uint8_t>> witness_signatures;       ///< Witness signatures
        quantum::QuantumState quantum_state;              ///< Quantum verification state
        vector<quantum::QuantumMeasurement> measurements; ///< Quantum measurements
        
        /**
         * @brief Zero-knowledge proof data for batch verification
         */
        struct ZKPData {
            vector<uint8_t> commitment;              ///< ZKP commitment
            vector<uint8_t> challenge;               ///< Verifier challenge
            vector<uint8_t> response;                ///< Prover response
            vector<uint8_t> recursive_proof;         ///< Recursive ZKP layer
            double verification_confidence{0.0};      ///< Confidence score (0-1)

            /**
             * @brief Checks if ZKP data is complete
             * @return true if all required data is present
             */
            [[nodiscard]] bool isComplete() const noexcept {
                return !commitment.empty() && 
                       !challenge.empty() && 
                       !response.empty() &&
                       verification_confidence >= 0.0 &&
                       verification_confidence <= 1.0;
            }
        } zkp_data;
        
        /**
         * @brief Witness-related data for batch verification
         */
        struct WitnessData {
            vector<string> selected_witnesses;        ///< IDs of selected witnesses
            vector<double> reliability_scores;        ///< Witness reliability scores
            vector<uint64_t> verification_times;      ///< Verification timestamps
            double quorum_threshold;                  ///< Required witness agreement
            bool has_consensus{false};                ///< Whether consensus reached

            /**
             * @brief Checks if witness quorum is reached
             * @return true if enough reliable witnesses have verified
             */
            [[nodiscard]] bool hasQuorum() const noexcept {
                if (selected_witnesses.empty() || reliability_scores.empty()) {
                    return false;
                }

                double total_weight = 0.0;
                double verified_weight = 0.0;

                for (size_t i = 0; i < reliability_scores.size(); ++i) {
                    total_weight += reliability_scores[i];
                    if (i < verification_times.size() && verification_times[i] > 0) {
                        verified_weight += reliability_scores[i];
                    }
                }

                return total_weight > 0.0 && 
                       (verified_weight / total_weight) >= quorum_threshold;
            }
        } witness_data;
        
        /**
         * @brief Performance metrics for batch processing
         */
        struct BatchMetrics {
            double avg_transaction_size{0.0};         ///< Average transaction size
            double proof_generation_time{0.0};        ///< Time to generate proof (ms)
            double verification_time{0.0};            ///< Time to verify proof (ms)
            size_t recursive_depth{0};                ///< Depth of recursive ZKP
            double quantum_enhancement_factor{1.0};    ///< Quantum speedup factor

            /**
             * @brief Checks if metrics are within acceptable ranges
             * @return true if metrics are valid
             */
            [[nodiscard]] bool isValid() const noexcept {
                return avg_transaction_size > 0.0 &&
                       proof_generation_time > 0.0 &&
                       verification_time > 0.0 &&
                       recursive_depth <= 5 && // Maximum practical recursive depth
                       quantum_enhancement_factor >= 1.0;
            }
        } metrics;
        
        /**
         * @brief Validates the batch proof structure
         * @return true if proof structure is valid
         */
        [[nodiscard]] bool isValid() const noexcept {
            return timestamp > 0 &&
                   transaction_count > 0 &&
                   !batch_hash.empty() &&
                   !proof_data.empty() &&
                   !witness_signatures.empty() &&
                   !witness_data.selected_witnesses.empty() &&
                   witness_data.reliability_scores.size() == witness_data.selected_witnesses.size() &&
                   witness_data.verification_times.size() == witness_data.selected_witnesses.size() &&
                   witness_data.quorum_threshold >= 0.66 && // 2/3 minimum for BFT
                   metrics.isValid();
        }
        
        /**
         * @brief Calculates the consensus confidence
         * @return Confidence score between 0 and 1
         */
        [[nodiscard]] double calculateConfidence() const noexcept {
            if (witness_signatures.empty() || witness_data.reliability_scores.empty()) {
                return 0.0;
            }
            
            // Weight witness signatures by reliability scores
            double total_weight = 0.0;
            double valid_weight = 0.0;
            
            for (size_t i = 0; i < witness_data.reliability_scores.size(); ++i) {
                const double reliability = witness_data.reliability_scores[i];
                total_weight += reliability;
                if (i < witness_signatures.size() && !witness_signatures[i].empty()) {
                    valid_weight += reliability;
                }
            }
            
            return total_weight > 0.0 ? valid_weight / total_weight : 0.0;
        }

        /**
         * @brief Checks if the batch proof is ready for consensus
         * @return true if proof can be used for consensus
         */
        [[nodiscard]] bool isReadyForConsensus() const noexcept {
            return isValid() && 
                   zkp_data.isComplete() &&
                   witness_data.hasQuorum() &&
                   calculateConfidence() >= witness_data.quorum_threshold;
        }
    };

    /**
     * @brief Consensus performance metrics
     */
    struct ConsensusMetrics {
        std::atomic<double> avg_batch_time{0.0};          ///< Average batch processing time (ms)
        std::atomic<double> avg_verification_time{0.0};   ///< Average verification time (ms)
        std::atomic<uint64_t> total_batches{0};          ///< Total batches processed
        std::atomic<uint64_t> total_transactions{0};      ///< Total transactions processed
        std::atomic<double> witness_participation{0.0};   ///< Witness participation rate
        std::atomic<double> quantum_security{0.0};        ///< Quantum security score
        std::atomic<double> quantum_fidelity{1.0};        ///< Quantum state fidelity
        std::atomic<uint64_t> error_corrections{0};       ///< Number of error corrections
        
        // Additional POBPC-specific metrics
        struct BatchMetrics {
            std::atomic<double> avg_batch_size{0.0};      ///< Average transactions per batch
            std::atomic<double> batch_formation_time{0.0}; ///< Time to form batches (ms)
            std::atomic<double> proof_generation_time{0.0}; ///< Time to generate proofs (ms)
            std::atomic<uint64_t> failed_batches{0};      ///< Number of failed batch proofs
            std::atomic<double> batch_success_rate{1.0};   ///< Successful batch ratio
        } batch_metrics;
        
        struct WitnessMetrics {
            std::atomic<double> avg_witness_count{0.0};    ///< Average witnesses per batch
            std::atomic<double> witness_response_time{0.0}; ///< Average response time (ms)
            std::atomic<uint64_t> witness_timeouts{0};     ///< Number of witness timeouts
            std::atomic<double> witness_reliability{1.0};   ///< Average witness reliability
            std::atomic<uint64_t> malicious_attempts{0};   ///< Detected malicious attempts
        } witness_metrics;
        
        struct ZKPMetrics {
            std::atomic<double> avg_proof_size{0.0};      ///< Average ZKP size (bytes)
            std::atomic<double> verification_success{1.0}; ///< ZKP verification success rate
            std::atomic<double> recursive_depth_avg{1.0};  ///< Average recursive ZKP depth
            std::atomic<uint64_t> proof_optimizations{0};  ///< Number of proof optimizations
            std::atomic<double> quantum_speedup{1.0};      ///< Quantum vs classical speedup
        } zkp_metrics;
        
        struct NetworkMetrics {
            std::atomic<double> consensus_latency{0.0};    ///< Average consensus time (ms)
            std::atomic<double> network_throughput{0.0};   ///< Transactions per second
            std::atomic<uint64_t> network_conflicts{0};    ///< Number of consensus conflicts
            std::atomic<double> bandwidth_usage{0.0};      ///< Average bandwidth (MB/s)
            std::atomic<double> sync_time{0.0};           ///< Average state sync time (ms)
        } network_metrics;
        
        /**
         * @brief Calculates overall system health score
         * @return Health score between 0 and 1
         */
        [[nodiscard]] double calculateHealthScore() const noexcept {
            // Weight different metrics based on importance
            constexpr double BATCH_WEIGHT = 0.25;
            constexpr double WITNESS_WEIGHT = 0.25;
            constexpr double ZKP_WEIGHT = 0.25;
            constexpr double NETWORK_WEIGHT = 0.25;
            
            // Calculate component scores
            const double batch_score = 
                batch_metrics.batch_success_rate * 
                (1.0 - std::min<double>(1.0, static_cast<double>(batch_metrics.failed_batches) / 
                                    std::max<double>(1.0, static_cast<double>(total_batches))));
            
            const double witness_score = 
                witness_metrics.witness_reliability * 
                (1.0 - std::min<double>(1.0, static_cast<double>(witness_metrics.witness_timeouts) / 
                                    std::max<double>(1.0, witness_metrics.avg_witness_count)));
            
            const double zkp_score = 
                zkp_metrics.verification_success * 
                zkp_metrics.quantum_speedup / std::max<double>(1.0, zkp_metrics.recursive_depth_avg);
            
            const double network_score = 
                (1.0 - std::min<double>(1.0, network_metrics.consensus_latency / 1000.0)) * // Normalize to seconds
                (1.0 - std::min<double>(1.0, static_cast<double>(network_metrics.network_conflicts) / 
                                    std::max<double>(1.0, static_cast<double>(total_batches))));
            
            // Combine scores with weights
            return batch_score * BATCH_WEIGHT +
                   witness_score * WITNESS_WEIGHT +
                   zkp_score * ZKP_WEIGHT +
                   network_score * NETWORK_WEIGHT;
        }
    };

    /**
     * @brief Creates a new POBPC instance
     * @param config Initial configuration
     */
    explicit OptimizedPOBPC(const BatchConfig& config);
    ~OptimizedPOBPC();

    // Disable copy and move
    OptimizedPOBPC(const OptimizedPOBPC&) = delete;
    OptimizedPOBPC& operator=(const OptimizedPOBPC&) = delete;
    OptimizedPOBPC(OptimizedPOBPC&&) = delete;
    OptimizedPOBPC& operator=(OptimizedPOBPC&&) = delete;

    // Core consensus operations
    /**
     * @brief Adds a transaction to the consensus pool
     * @param transaction Transaction data
     * @return true if transaction was added
     */
    [[nodiscard]] bool addTransaction(const std::vector<uint8_t>& transaction);

    /**
     * @brief Generates proof for current batch
     * @return Batch proof if successful
     */
    [[nodiscard]] BatchProof generateBatchProof();

    /**
     * @brief Verifies a batch proof
     * @param proof Proof to verify
     * @return true if proof is valid
     */
    [[nodiscard]] bool verifyBatchProof(const BatchProof& proof) const;

    // Witness management
    /**
     * @brief Registers a new witness
     * @param node_id Witness node ID
     * @param public_key Witness public key
     * @return true if registration successful
     */
    [[nodiscard]] bool registerWitness(
        const std::string& node_id,
        const std::vector<uint8_t>& public_key);

    /**
     * @brief Selects witnesses for current batch
     * @return Vector of selected witnesses
     */
    [[nodiscard]] std::vector<WitnessInfo> selectWitnesses() const;

    /**
     * @brief Submits a witness vote
     * @param witness_id Witness ID
     * @param signature Vote signature
     * @param proof Batch proof being voted on
     * @return true if vote was accepted
     */
    [[nodiscard]] bool submitWitnessVote(
        const std::string& witness_id,
        const std::vector<uint8_t>& signature,
        const BatchProof& proof);

    // Consensus status
    /**
     * @brief Checks if consensus reached
     * @param proof Batch proof to check
     * @return true if consensus reached
     */
    [[nodiscard]] bool hasReachedConsensus(const BatchProof& proof) const noexcept;

    /**
     * @brief Calculates consensus confidence
     * @param proof Batch proof to analyze
     * @return Confidence value between 0 and 1
     */
    [[nodiscard]] double calculateConsensusConfidence(const BatchProof& proof) const noexcept;

    /**
     * @brief Gets current consensus metrics
     * @return Current metrics
     */
    [[nodiscard]] ConsensusMetrics getMetrics() const noexcept;

    // Configuration
    /**
     * @brief Updates consensus configuration
     * @param config New configuration
     */
    void updateConfig(const BatchConfig& config);

    /**
     * @brief Gets current configuration
     * @return Current configuration
     */
    [[nodiscard]] BatchConfig getConfig() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

    // SIMD-optimized batch processing
    static constexpr std::size_t CACHE_LINE_SIZE = 64;
    static constexpr std::size_t SIMD_WIDTH = 8;
    static constexpr double MIN_RELIABILITY = 0.5;
    static constexpr std::size_t MAX_QUANTUM_DEPTH = 100;
    static constexpr double ERROR_THRESHOLD = 1e-6;

    alignas(CACHE_LINE_SIZE) struct BatchBuffer {
        static constexpr std::size_t BUFFER_SIZE = 1024;
        std::array<std::vector<uint8_t>, BUFFER_SIZE> transactions;
        std::atomic<std::size_t> count{0};
        std::atomic<bool> processing{false};
    };

    // Lock-free transaction management
    quids::utils::LockFreeQueue<std::vector<uint8_t>> transaction_queue_;

    // Quantum security context
    struct QuantumContext {
        quantum::QuantumState consensus_state;
        quantum::QuantumCircuit verification_circuit;
        std::vector<quantum::QuantumMeasurement> measurements;
        std::atomic<double> entanglement_score{0.0};
        std::atomic<double> coherence_score{0.0};
    };

    // Helper functions
    [[nodiscard]] std::vector<uint8_t> createBatchHash(
        const std::vector<std::vector<uint8_t>>& transactions) const;

    [[nodiscard]] bool verifyWitnessSignature(
        const std::string& witness_id,
        const std::vector<uint8_t>& signature,
        const std::vector<uint8_t>& message) const;

    void updateWitnessReliability(
        const std::string& witness_id,
        bool successful_validation);

    [[nodiscard]] std::vector<WitnessInfo> selectWitnessesRandomly(
        std::size_t count) const;

    [[nodiscard]] bool validateBatchStructure(
        const BatchProof& proof) const noexcept;

    void recordMetrics(
        const BatchProof& proof,
        std::chrono::microseconds processing_time);

    // SIMD batch processing
    void processBatchSIMD(std::vector<std::vector<uint8_t>>& batch);
    void verifyBatchSIMD(const BatchProof& proof, std::vector<bool>& results);

    // Quantum operations
    [[nodiscard]] quantum::QuantumState generateQuantumProof(
        const std::vector<std::vector<uint8_t>>& transactions) const;

    [[nodiscard]] bool verifyQuantumProof(
        const quantum::QuantumState& proof_state,
        const std::vector<uint8_t>& batch_hash) const;

    void updateQuantumState(const BatchProof& proof);
    [[nodiscard]] double calculateQuantumSecurityScore(const BatchProof& proof) const noexcept;

    // Error correction
    void applyQuantumErrorCorrection();
    [[nodiscard]] bool detectQuantumErrors() const noexcept;
    void recoverFromErrors();
};

} // namespace quids::consensus

#endif // QUIDS_CONSENSUS_OPTIMIZED_POBPC_HPP 