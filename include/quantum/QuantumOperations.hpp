#ifndef QUIDS_QUANTUM_OPERATIONS_HPP
#define QUIDS_QUANTUM_OPERATIONS_HPP

#include "QuantumTypes.hpp"
#include "QuantumState.hpp"
#include <complex>
#include <cstddef>

namespace quids::quantum {

using GateMatrix = ::quids::quantum::GateMatrix;
using OperatorMatrix = ::quids::quantum::OperatorMatrix;
using QuantumState = ::quids::quantum::QuantumState;

namespace operations {

/**
 * @brief Namespace containing quantum operations and gates
 * 
 * This namespace provides functions for creating and manipulating quantum
 * gates and operations used in quantum computation. All gates are represented
 * as unitary matrices operating on quantum states.
 */

///@{
/** @name Single-qubit Operations
 * Functions for creating basic single-qubit quantum gates
 */

/**
 * @brief Creates Hadamard gate matrix
 * @return 2x2 Hadamard gate matrix H = 1/√2 * [1  1]
 *                                          [1 -1]
 */
[[nodiscard]] GateMatrix createHadamard() noexcept;

/**
 * @brief Creates Pauli-X (NOT) gate matrix
 * @return 2x2 Pauli-X gate matrix X = [0 1]
 *                                     [1 0]
 */
[[nodiscard]] GateMatrix createPauliX() noexcept;

/**
 * @brief Creates Pauli-Y gate matrix
 * @return 2x2 Pauli-Y gate matrix Y = [0 -i]
 *                                     [i  0]
 */
[[nodiscard]] GateMatrix createPauliY() noexcept;

/**
 * @brief Creates Pauli-Z gate matrix
 * @return 2x2 Pauli-Z gate matrix Z = [1  0]
 *                                     [0 -1]
 */
[[nodiscard]] GateMatrix createPauliZ() noexcept;

/**
 * @brief Creates phase gate matrix
 * @param angle Phase angle in radians
 * @return 2x2 phase gate matrix P(θ) = [1 0   ]
 *                                      [0 e^iθ]
 */
[[nodiscard]] GateMatrix createPhase(double angle) noexcept;

/**
 * @brief Creates rotation gate around X axis
 * @param angle Rotation angle in radians
 * @return 2x2 rotation matrix Rx(θ)
 */
[[nodiscard]] GateMatrix createRotationX(double angle) noexcept;

/**
 * @brief Creates rotation gate around Y axis
 * @param angle Rotation angle in radians
 * @return 2x2 rotation matrix Ry(θ)
 */
[[nodiscard]] GateMatrix createRotationY(double angle) noexcept;

/**
 * @brief Creates rotation gate around Z axis
 * @param angle Rotation angle in radians
 * @return 2x2 rotation matrix Rz(θ)
 */
[[nodiscard]] GateMatrix createRotationZ(double angle) noexcept;

///@}

///@{
/** @name Multi-qubit Operations
 * Functions for creating multi-qubit quantum gates
 */

/**
 * @brief Creates CNOT (Controlled-NOT) gate matrix
 * @return 4x4 CNOT gate matrix
 */
[[nodiscard]] OperatorMatrix createCNOT() noexcept;

/**
 * @brief Creates SWAP gate matrix
 * @return 4x4 SWAP gate matrix
 */
[[nodiscard]] OperatorMatrix createSWAP() noexcept;

/**
 * @brief Creates Toffoli (CCNOT) gate matrix
 * @return 8x8 Toffoli gate matrix
 */
[[nodiscard]] OperatorMatrix createToffoli() noexcept;

/**
 * @brief Creates controlled phase gate matrix
 * @param angle Phase angle in radians
 * @return 4x4 controlled phase gate matrix
 */
[[nodiscard]] OperatorMatrix createControlledPhase(double angle) noexcept;

/**
 * @brief Creates controlled-U gate from single-qubit gate
 * @param u Single-qubit unitary to control
 * @return 4x4 controlled-U gate matrix
 */
[[nodiscard]] OperatorMatrix createControlledU(const GateMatrix& u);

///@}

///@{
/** @name Gate Operations
 * Functions for manipulating and combining quantum gates
 */

/**
 * @brief Tensor product of two gates
 * @param a First gate
 * @param b Second gate
 * @return Combined gate matrix
 */
[[nodiscard]] OperatorMatrix tensorProduct(
    const OperatorMatrix& a,
    const OperatorMatrix& b);

/**
 * @brief Creates controlled version of a gate
 * @param gate Gate to control
 * @param numQubits Total number of qubits
 * @param control Control qubit index
 * @param target Target qubit index
 * @return Controlled gate matrix
 */
[[nodiscard]] OperatorMatrix controlGate(
    const OperatorMatrix& gate,
    std::size_t numQubits,
    std::size_t control,
    std::size_t target);

/**
 * @brief Applies a gate to a quantum state
 * @param state State to modify
 * @param gate Gate to apply
 * @param target Target qubit index
 */
void applyGate(
    QuantumState& state,
    const GateMatrix& gate,
    std::size_t target);

/**
 * @brief Applies a controlled gate to a quantum state
 * @param state State to modify
 * @param gate Gate to apply
 * @param control Control qubit index
 * @param target Target qubit index
 */
void applyControlledGate(
    QuantumState& state,
    const GateMatrix& gate,
    std::size_t control,
    std::size_t target);

///@}

} // namespace operations
} // namespace quids::quantum

#endif // QUIDS_QUANTUM_OPERATIONS_HPP 