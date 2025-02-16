#ifndef QUIDS_QUANTUM_QUANTUM_GATES_HPP
#define QUIDS_QUANTUM_QUANTUM_GATES_HPP

#include "StdNamespace.hpp"
#include "QuantumTypes.hpp"
#include <Eigen/Dense>
#include <complex>
#include <vector>
#include <cmath>

namespace quids::quantum {

/**
 * @brief Quantum gate factory and manipulation utilities
 * 
 * This namespace contains functions for creating and manipulating quantum gates,
 * including standard gates, controlled operations, and custom unitary matrices.
 * All gates are represented as unitary matrices that preserve quantum state norms.
 */
namespace gates {

/**
 * @brief Standard single-qubit gates
 * 
 * Pre-defined constant matrices for commonly used single-qubit quantum gates.
 * These gates form a basis for quantum computation and are used as building
 * blocks for more complex operations.
 */
namespace standard {
    /// Hadamard gate: Creates superposition
    static const GateMatrix H = (GateMatrix(2,2) << 
        1/std::sqrt(2),  1/std::sqrt(2),
        1/std::sqrt(2), -1/std::sqrt(2)).finished();

    /// Pauli-X gate: Quantum NOT operation
    static const GateMatrix X = (GateMatrix(2,2) << 
        0, 1,
        1, 0).finished();

    /// Pauli-Y gate: Complex rotation
    static const GateMatrix Y = (GateMatrix(2,2) << 
        0, Complex(0,-1),
        Complex(0,1), 0).finished();

    /// Pauli-Z gate: Phase flip
    static const GateMatrix Z = (GateMatrix(2,2) << 
        1,  0,
        0, -1).finished();

    /// S gate: √Z phase gate
    static const GateMatrix S = (GateMatrix(2,2) << 
        1, 0,
        0, Complex(0,1)).finished();

    /// T gate: π/4 phase gate
    static const GateMatrix T = (GateMatrix(2,2) << 
        1, 0,
        0, std::exp(Complex(0, M_PI/4))).finished();
} // namespace standard

/**
 * @brief Creates a rotation gate around specified axis
 * @param axis Axis of rotation (0=X, 1=Y, 2=Z)
 * @param angle Rotation angle in radians
 * @return 2x2 rotation gate matrix
 * @throws std::invalid_argument if axis is invalid
 */
[[nodiscard]] GateMatrix createRotation(std::size_t axis, double angle);

/**
 * @brief Creates an arbitrary single-qubit unitary gate
 * @param theta First Euler angle
 * @param phi Second Euler angle
 * @param lambda Third Euler angle
 * @return 2x2 unitary gate matrix
 */
[[nodiscard]] GateMatrix createUnitary(double theta, double phi, double lambda) noexcept;

/**
 * @brief Creates a controlled version of a single-qubit gate
 * @param gate Single-qubit gate to control
 * @return 4x4 controlled gate matrix
 */
[[nodiscard]] OperatorMatrix createControlled(const GateMatrix& gate) noexcept;

/**
 * @brief Creates a tensor product of multiple gates
 * @param gates Vector of gate matrices to combine
 * @return Combined gate matrix
 */
[[nodiscard]] OperatorMatrix tensorProduct(const std::vector<GateMatrix>& gates);

/**
 * @brief Checks if a matrix represents a valid quantum gate
 * @param matrix Matrix to check
 * @param tolerance Maximum allowed deviation from unitarity
 * @return true if matrix is unitary within tolerance
 */
[[nodiscard]] bool isValidGate(
    const OperatorMatrix& matrix,
    double tolerance = constants::QUANTUM_ERROR_THRESHOLD) noexcept;

/**
 * @brief Decomposes a unitary matrix into basic gates
 * @param unitary Unitary matrix to decompose
 * @return Vector of basic gates that approximate the unitary
 * @throws std::invalid_argument if matrix is not unitary
 */
[[nodiscard]] std::vector<GateOperation> decomposeUnitary(const OperatorMatrix& unitary);

/**
 * @brief Optimizes a sequence of quantum gates
 * @param gates Vector of gate operations
 * @return Optimized sequence of gates
 */
[[nodiscard]] std::vector<GateOperation> optimizeSequence(
    const std::vector<GateOperation>& gates);

/**
 * @brief Calculates the fidelity between two quantum gates
 * @param gate1 First gate matrix
 * @param gate2 Second gate matrix
 * @return Fidelity between gates (0 to 1)
 */
[[nodiscard]] double calculateFidelity(
    const OperatorMatrix& gate1,
    const OperatorMatrix& gate2) noexcept;

namespace detail {
    /**
     * @brief Helper function to create tensor product of matrices
     * @param matrices Vector of matrices to combine
     * @return Tensor product result
     */
    [[nodiscard]] OperatorMatrix tensorProduct(
        const std::vector<OperatorMatrix>& matrices) noexcept;

    /**
     * @brief Helper function to create controlled version of a matrix
     * @param matrix Matrix to control
     * @return Controlled version of input matrix
     */
    [[nodiscard]] OperatorMatrix makeControlled(
        const OperatorMatrix& matrix) noexcept;
} // namespace detail

} // namespace gates
} // namespace quids::quantum

#endif // QUIDS_QUANTUM_QUANTUM_GATES_HPP