#ifndef QUIDS_QUANTUM_QUANTUM_UTILS_HPP
#define QUIDS_QUANTUM_QUANTUM_UTILS_HPP

#include "QuantumTypes.hpp"
#include "QuantumState.hpp"
#include <Eigen/Dense>
#include <vector>
#include <complex>
#include <optional>
#include <random>
#include <cmath>

namespace quids::quantum {

/**
 * @brief Utility functions for quantum computation
 * 
 * This namespace contains helper functions for quantum state manipulation,
 * measurement, and analysis that are used across the quantum computing module.
 */
namespace utils {

/**
 * @brief Creates tensor product of two quantum states
 * @param a First quantum state
 * @param b Second quantum state
 * @return Combined quantum state
 */
[[nodiscard]] QuantumState tensorProduct(
    const QuantumState& a, 
    const QuantumState& b);

/**
 * @brief Performs partial trace over specified qubit
 * @param state Input quantum state
 * @param qubit_index Qubit to trace out
 * @param num_qubits Total number of qubits
 * @return Reduced quantum state
 */
[[nodiscard]] QuantumState partialTrace(
    const QuantumState& state, 
    std::size_t qubit_index, 
    std::size_t num_qubits);

/**
 * @brief Measures a single qubit in computational basis
 * @param state Quantum state to measure (modified by measurement)
 * @param qubit_index Qubit to measure
 * @return Pair of (measurement outcome, probability)
 */
[[nodiscard]] std::pair<std::size_t, double> measureQubit(
    QuantumState& state, 
    std::size_t qubit_index);

/**
 * @brief Gets measurement probabilities for all basis states
 * @param state Quantum state to analyze
 * @return Vector of measurement probabilities
 */
[[nodiscard]] std::vector<double> getMeasurementProbabilities(
    const QuantumState& state);

// State preparation functions
/**
 * @brief Creates a Bell state (maximally entangled two-qubit state)
 * @return Bell state |Φ⁺⟩ = (|00⟩ + |11⟩)/√2
 */
[[nodiscard]] QuantumState createBellPair();

/**
 * @brief Creates a GHZ state
 * @param num_qubits Number of qubits in state
 * @return GHZ state (|00...0⟩ + |11...1⟩)/√2
 */
[[nodiscard]] QuantumState createGHZState(std::size_t num_qubits);

/**
 * @brief Creates a W state
 * @param num_qubits Number of qubits in state
 * @return W state (|100...0⟩ + |010...0⟩ + ... + |000...1⟩)/√n
 */
[[nodiscard]] QuantumState createWState(std::size_t num_qubits);

// Error correction
/**
 * @brief Applies error correction to quantum state
 * @param state State with errors
 * @param syndrome Error syndrome
 * @return Corrected quantum state
 */
[[nodiscard]] QuantumState applyErrorCorrection(
    const QuantumState& state, 
    const ErrorSyndrome& syndrome);

/**
 * @brief Detects errors in quantum state
 * @param state State to check for errors
 * @return Error syndrome containing error information
 */
[[nodiscard]] ErrorSyndrome detectErrors(const QuantumState& state);

// Quantum state metrics
/**
 * @brief Calculates fidelity between two states
 * @param state1 First state
 * @param state2 Second state
 * @return Fidelity value between 0 and 1
 */
[[nodiscard]] double calculateFidelity(
    const QuantumState& state1, 
    const QuantumState& state2) noexcept;

/**
 * @brief Calculates trace distance between states
 * @param state1 First state
 * @param state2 Second state
 * @return Trace distance value between 0 and 1
 */
[[nodiscard]] double calculateTraceDistance(
    const QuantumState& state1, 
    const QuantumState& state2) noexcept;

/**
 * @brief Calculates von Neumann entropy
 * @param state Input quantum state
 * @return Entropy value
 */
[[nodiscard]] double calculateVonNeumannEntropy(const QuantumState& state) noexcept;

// Helper functions
/**
 * @brief Gets dimension of state space
 * @param num_qubits Number of qubits
 * @return Dimension (2^n for n qubits)
 */
[[nodiscard]] std::size_t getStateDimension(std::size_t num_qubits) noexcept;

/**
 * @brief Gets computational basis state indices
 * @param num_qubits Number of qubits
 * @return Vector of basis state indices
 */
[[nodiscard]] std::vector<std::size_t> getComputationalBasisStates(
    std::size_t num_qubits);

/**
 * @brief Checks if matrix is unitary
 * @param matrix Matrix to check
 * @return true if matrix is unitary
 */
[[nodiscard]] bool isUnitary(const OperatorMatrix& matrix) noexcept;

// SIMD-optimized operations
namespace simd {
    /**
     * @brief Applies single-qubit gate using SIMD
     * @param state State vector to modify
     * @param gate Gate matrix to apply
     * @param qubit_index Target qubit
     */
    void applySingleQubitGate(
        StateVector& state,
        const OperatorMatrix& gate,
        std::size_t qubit_index);

    /**
     * @brief Applies two-qubit gate using SIMD
     * @param state State vector to modify
     * @param gate Gate matrix to apply
     * @param qubit1 First qubit
     * @param qubit2 Second qubit
     */
    void applyTwoQubitGate(
        StateVector& state,
        const OperatorMatrix& gate,
        std::size_t qubit1,
        std::size_t qubit2);

    /**
     * @brief Applies controlled gate using SIMD
     * @param state State vector to modify
     * @param gate Gate matrix to apply
     * @param control Control qubit
     * @param target Target qubit
     */
    void applyControlledGate(
        StateVector& state,
        const OperatorMatrix& gate,
        std::size_t control,
        std::size_t target);
} // namespace simd

} // namespace utils

namespace detail {
    /**
     * @brief Internal helper functions for quantum utilities
     */

    [[nodiscard]] double calculateFidelity(
        const QuantumState& state1,
        const QuantumState& state2) noexcept;

    [[nodiscard]] double calculateEntanglement(
        const QuantumState& state) noexcept;

    [[nodiscard]] ErrorSyndrome detectErrors(
        const QuantumState& state);

    [[nodiscard]] QuantumState correctErrors(
        const QuantumState& state,
        const ErrorSyndrome& syndrome);

    [[nodiscard]] double calculateQuantumSecurity(
        const QuantumState& state) noexcept;

} // namespace detail

} // namespace quids::quantum

#endif // QUIDS_QUANTUM_QUANTUM_UTILS_HPP

