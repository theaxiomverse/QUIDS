#ifndef QUIDS_QUANTUM_QUANTUM_CONSENSUS_HPP
#define QUIDS_QUANTUM_QUANTUM_CONSENSUS_HPP

#include "StdNamespace.hpp"
#include "QuantumTypes.hpp"
#include "QuantumState.hpp"
#include "QuantumCircuit.hpp"
#include <vector>
#include <optional>

namespace quids::quantum {

// Common type declarations from quids namespace
using std::size_t;
using std::vector;
using std::optional;

/**
 * @brief Quantum consensus protocols and utilities
 * 
 * This namespace provides implementations of quantum consensus protocols,
 * including quantum Byzantine agreement, quantum leader election, and
 * quantum distributed key generation.
 */
namespace consensus {

/**
 * @brief Configuration for quantum consensus protocols
 */
struct ConsensusConfig {
    size_t num_parties{3};                    ///< Number of participating parties
    size_t num_qubits_per_party{4};          ///< Number of qubits per party
    double error_threshold{1e-6};                  ///< Error tolerance threshold
    size_t max_rounds{100};                  ///< Maximum consensus rounds
    bool use_entanglement{true};                  ///< Whether to use entanglement
    double timeout_seconds{30.0};                 ///< Timeout in seconds
};

/**
 * @brief Result of a quantum consensus protocol
 */
struct ConsensusResult {
    bool success{false};                          ///< Whether consensus was reached
    size_t rounds_taken{0};                 ///< Number of rounds taken
    vector<QuantumState> final_states{};    ///< Final quantum states
    double fidelity{0.0};                        ///< Protocol fidelity
    vector<size_t> measurements{};     ///< Final measurements
};

/**
 * @brief Implements quantum Byzantine agreement protocol
 * @param config Protocol configuration
 * @param initial_states Initial quantum states of parties
 * @return Result of the consensus protocol
 */
[[nodiscard]] ConsensusResult quantumByzantineAgreement(
    const ConsensusConfig& config,
    const vector<QuantumState>& initial_states);

/**
 * @brief Implements quantum leader election protocol
 * @param config Protocol configuration
 * @return Result containing elected leader and protocol metrics
 */
[[nodiscard]] ConsensusResult quantumLeaderElection(
    const ConsensusConfig& config);

/**
 * @brief Implements quantum distributed key generation
 * @param config Protocol configuration
 * @return Result containing generated key and protocol metrics
 */
[[nodiscard]] ConsensusResult quantumDistributedKeyGeneration(
    const ConsensusConfig& config);

/**
 * @brief Verifies the correctness of a quantum consensus result
 * @param result Result to verify
 * @param config Configuration used
 * @return true if result is valid
 */
[[nodiscard]] bool verifyConsensus(
    const ConsensusResult& result,
    const ConsensusConfig& config) noexcept;

namespace detail {
    /**
     * @brief Creates entangled states for consensus protocol
     * @param num_parties Number of parties
     * @param qubits_per_party Qubits per party
     * @return Vector of entangled quantum states
     */
    [[nodiscard]] vector<QuantumState> createEntangledStates(
        size_t num_parties,
        size_t qubits_per_party);

    /**
     * @brief Performs quantum voting round
     * @param states Current quantum states
     * @param round Round number
     * @return Vector of measurement results
     */
    [[nodiscard]] vector<size_t> performVotingRound(
        vector<QuantumState>& states,
        size_t round);

    /**
     * @brief Checks if consensus has been reached
     * @param measurements Vector of measurements
     * @param threshold Required agreement threshold
     * @return true if consensus reached
     */
    [[nodiscard]] bool checkConsensus(
        const vector<size_t>& measurements,
        double threshold) noexcept;
} // namespace detail

} // namespace consensus
} // namespace quids::quantum

#endif // QUIDS_QUANTUM_QUANTUM_CONSENSUS_HPP 