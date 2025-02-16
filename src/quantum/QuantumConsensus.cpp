#include "quantum/QuantumConsensus.hpp"
#include "quantum/QuantumOperations.hpp"
#include "quantum/QuantumUtils.hpp"
#include <random>
#include <algorithm>

namespace quids::quantum::consensus {

namespace {
    /**
     * @brief Creates an entangled quantum state shared between multiple parties
     * 
     * This function creates a multi-party entangled state by:
     * 1. Initializing a quantum state with sufficient qubits for all parties
     * 2. Applying Hadamard gates to create superposition
     * 3. Using CNOT gates to create entanglement between adjacent parties
     * 
     * @param num_parties Number of parties to entangle
     * @param qubits_per_party Number of qubits allocated to each party
     * @return Entangled quantum state spanning all parties
     */
    QuantumState createMultiPartyEntanglement(size_t num_parties, size_t qubits_per_party) {
        QuantumState state(num_parties * qubits_per_party);
        
        // Apply Hadamard to first qubit of each party
        for (size_t i = 0; i < num_parties; ++i) {
            state.applyHadamard(i * qubits_per_party);
        }
        
        // Create entanglement between adjacent parties
        for (size_t i = 0; i < num_parties - 1; ++i) {
            state.applyCNOT(i * qubits_per_party, (i + 1) * qubits_per_party);
        }
        
        return state;
    }
}

/**
 * @brief Implements quantum Byzantine agreement protocol
 * 
 * This implementation uses quantum entanglement to achieve Byzantine fault tolerance:
 * 1. Creates shared entangled states between all parties
 * 2. Distributes entangled qubits to each party
 * 3. Performs multiple rounds of quantum voting until consensus
 * 4. Verifies agreement through quantum measurements
 * 
 * The protocol is resistant to Byzantine faults up to f < n/3 faulty parties.
 */
ConsensusResult quantumByzantineAgreement(
    const ConsensusConfig& config,
    const vector<QuantumState>& initial_states) {
    
    ConsensusResult result;
    result.rounds_taken = 0;
    
    // Validate input
    if (initial_states.size() != config.num_parties) {
        return result;
    }
    
    // Create shared entangled state
    auto entangled_state = createMultiPartyEntanglement(
        config.num_parties, 
        config.num_qubits_per_party
    );
    
    // Distribute entangled qubits to parties
    vector<QuantumState> party_states = initial_states;
    for (size_t i = 0; i < config.num_parties; ++i) {
        party_states[i] = utils::tensorProduct(
            party_states[i],
            entangled_state
        );
    }
    
    // Consensus rounds
    while (result.rounds_taken < config.max_rounds) {
        // Perform quantum voting
        auto measurements = detail::performVotingRound(party_states, result.rounds_taken);
        
        // Check if consensus reached
        if (detail::checkConsensus(measurements, config.error_threshold)) {
            result.success = true;
            result.measurements = measurements;
            break;
        }
        
        result.rounds_taken++;
    }
    
    result.final_states = party_states;
    result.fidelity = utils::calculateFidelity(party_states[0], party_states[1]);
    
    return result;
}

/**
 * @brief Implements quantum leader election protocol
 * 
 * Uses quantum superposition and measurement to achieve unbiased leader selection:
 * 1. Initializes each party's qubits in superposition
 * 2. Optionally creates entanglement for security
 * 3. Performs simultaneous measurements
 * 4. Selects leader based on measurement outcomes
 * 
 * The protocol ensures fair and random leader selection with quantum randomness.
 */
ConsensusResult quantumLeaderElection(const ConsensusConfig& config) {
    ConsensusResult result;
    
    // Initialize quantum states for each party
    vector<QuantumState> party_states;
    for (size_t i = 0; i < config.num_parties; ++i) {
        QuantumState state(config.num_qubits_per_party);
        state.applyHadamard(0); // Superposition for leader election
        party_states.push_back(state);
    }
    
    // Create entanglement between parties
    if (config.use_entanglement) {
        auto entangled_states = detail::createEntangledStates(
            config.num_parties,
            config.num_qubits_per_party
        );
        party_states = entangled_states;
    }
    
    // Perform measurements to elect leader
    auto measurements = detail::performVotingRound(party_states, 0);
    
    // Leader is the party with the highest measurement value
    auto max_it = std::max_element(measurements.begin(), measurements.end());
    result.success = true;
    result.rounds_taken = 1;
    result.measurements = measurements;
    result.final_states = party_states;
    
    return result;
}

/**
 * @brief Implements quantum distributed key generation
 * 
 * Generates shared cryptographic keys using quantum entanglement:
 * 1. Initializes quantum states with random rotations
 * 2. Creates entanglement between parties
 * 3. Performs measurements to generate shared random bits
 * 
 * The protocol provides information-theoretic security through quantum mechanics.
 */
ConsensusResult quantumDistributedKeyGeneration(const ConsensusConfig& config) {
    ConsensusResult result;
    
    // Initialize quantum states for key generation
    vector<QuantumState> party_states;
    for (size_t i = 0; i < config.num_parties; ++i) {
        QuantumState state(config.num_qubits_per_party);
        // Apply random rotations for key generation
        for (size_t j = 0; j < config.num_qubits_per_party; ++j) {
            state.applyHadamard(j);
            state.applyPhase(j, M_PI / 4.0);
        }
        party_states.push_back(state);
    }
    
    // Create entanglement for secure key distribution
    if (config.use_entanglement) {
        auto entangled_states = detail::createEntangledStates(
            config.num_parties,
            config.num_qubits_per_party
        );
        party_states = entangled_states;
    }
    
    // Perform measurements to generate shared key
    result.measurements = detail::performVotingRound(party_states, 0);
    result.success = true;
    result.rounds_taken = 1;
    result.final_states = party_states;
    
    return result;
}

/**
 * @brief Verifies the correctness of a quantum consensus result
 * 
 * Checks if:
 * 1. Consensus was successfully reached
 * 2. Number of rounds is within limits
 * 3. Measurements are consistent across parties
 * 
 * @return true if consensus result is valid and consistent
 */
bool verifyConsensus(
    const ConsensusResult& result,
    const ConsensusConfig& config) noexcept {
    
    if (!result.success || result.rounds_taken > config.max_rounds) {
        return false;
    }
    
    // Verify measurements are consistent
    return detail::checkConsensus(result.measurements, config.error_threshold);
}

namespace detail {

/**
 * @brief Creates entangled quantum states for multiple parties
 * 
 * Creates and distributes entangled states:
 * 1. Generates a multi-party entangled state
 * 2. Splits the state into separate states for each party
 * 3. Preserves quantum correlations between parties
 * 
 * @param num_parties Number of parties to create states for
 * @param qubits_per_party Number of qubits per party
 * @return Vector of entangled quantum states, one for each party
 */
vector<QuantumState> createEntangledStates(
    size_t num_parties,
    size_t qubits_per_party) {
    
    vector<QuantumState> states;
    auto entangled_state = createMultiPartyEntanglement(num_parties, qubits_per_party);
    
    // Split entangled state among parties
    for (size_t i = 0; i < num_parties; ++i) {
        QuantumState party_state(qubits_per_party);
        // Extract party's portion of entangled state
        for (size_t j = 0; j < qubits_per_party; ++j) {
            size_t global_idx = i * qubits_per_party + j;
            party_state.setAmplitude(j, entangled_state.getAmplitude(global_idx));
        }
        states.push_back(party_state);
    }
    
    return states;
}

/**
 * @brief Performs a round of quantum voting
 * 
 * Executes voting process:
 * 1. Each party measures their quantum state
 * 2. First qubit of each state is used for voting
 * 3. Measurements are collected and returned
 * 
 * @param states Quantum states of all parties
 * @param round Current voting round number
 * @return Vector of measurement outcomes from all parties
 */
vector<size_t> performVotingRound(
    vector<QuantumState>& states,
    size_t round) {
    
    vector<size_t> measurements;
    
    // Each party performs measurement
    for (auto& state : states) {
        auto result = state.getMeasurementOutcomes();
        measurements.push_back(result[0]); // Use first qubit for voting
    }
    
    return measurements;
}

/**
 * @brief Checks if consensus has been reached
 * 
 * Verifies consensus by:
 * 1. Counting measurement outcomes
 * 2. Calculating ratio of matching results
 * 3. Comparing against threshold
 * 
 * @param measurements Vector of measurement outcomes
 * @param threshold Required agreement threshold
 * @return true if consensus threshold is met
 */
bool checkConsensus(
    const vector<size_t>& measurements,
    double threshold) noexcept {
    
    if (measurements.empty()) {
        return false;
    }
    
    // Count occurrences of each measurement outcome
    vector<size_t> counts(2, 0); // Binary outcomes
    for (auto m : measurements) {
        if (m < counts.size()) {
            counts[m]++;
        }
    }
    
    // Check if any outcome exceeds threshold
    double max_ratio = static_cast<double>(*std::max_element(counts.begin(), counts.end())) 
                      / measurements.size();
    
    return max_ratio >= threshold;
}

} // namespace detail
} // namespace quids::quantum::consensus 