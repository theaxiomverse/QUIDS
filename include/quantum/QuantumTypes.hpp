#ifndef QUIDS_QUANTUM_TYPES_HPP
#define QUIDS_QUANTUM_TYPES_HPP

#include <Eigen/Dense>
#include <cstdint>
#include <vector>
#include <complex>
#include <unordered_map>
#include <functional>
#include <string>
#include <cstddef>

namespace quids::quantum {

/**
 * @brief Common type definitions for quantum computation
 */

/// Complex number type used throughout quantum computations
using Complex = ::std::complex<double>;

/// State vector type representing quantum states
using StateVector = Eigen::VectorXcd;

/// Operator matrix type for quantum gates
using OperatorMatrix = Eigen::MatrixXcd;

/// Vector type for measurement probabilities
using ProbabilityVector = ::std::vector<double>;

/// Gate matrix type (2x2 complex matrix)
using GateMatrix = Eigen::Matrix2cd;

/**
 * @brief Types of quantum gates available in the system
 */
enum class GateType {
    HADAMARD,   ///< Hadamard gate for superposition
    PAULI_X,    ///< Pauli-X (NOT) gate
    PAULI_Y,    ///< Pauli-Y gate
    PAULI_Z,    ///< Pauli-Z (phase) gate
    CNOT,       ///< Controlled-NOT gate
    SWAP,       ///< SWAP gate
    TOFFOLI,    ///< Toffoli (CCNOT) gate
    PHASE,      ///< Phase rotation gate
    ROTATION,   ///< Arbitrary rotation gate
    CUSTOM      ///< Custom unitary gate
};

/// Map type for storing gate parameters
using GateParameters = ::std::unordered_map<::std::string, double>;

/// Function type for custom gate operations
using GateFunction = ::std::function<GateMatrix(const GateParameters&)>;

/// Type for qubit indices
using QubitIndex = ::std::size_t;

/// Vector of qubit indices
using QubitIndices = ::std::vector<QubitIndex>;

/// Map of gate names to their matrices
using GateMap = ::std::unordered_map<::std::string, GateMatrix>;

/// Map of gate names to their parameters
using GateParameterMap = ::std::unordered_map<::std::string, GateParameters>;

/// Map of gate names to their functions
using GateFunctionMap = ::std::unordered_map<::std::string, GateFunction>;

/// Vector of gate operations
using GateOperations = ::std::vector<::std::pair<GateType, GateParameters>>;

/// Vector of measurement results
using MeasurementResults = ::std::vector<::std::pair<QubitIndex, bool>>;

/// Map of measurement probabilities
using MeasurementProbabilities = ::std::unordered_map<::std::string, double>;

/// Vector of complex amplitudes
using AmplitudeVector = ::std::vector<Complex>;

/// Map of basis states to amplitudes
using BasisStateMap = ::std::unordered_map<::std::string, Complex>;

/// Vector of quantum circuit layers
using CircuitLayers = ::std::vector<GateOperations>;

/// Map of circuit parameters
using CircuitParameters = ::std::unordered_map<::std::string, double>;

/// Vector of circuit metrics
using CircuitMetrics = ::std::vector<double>;

/// Map of circuit statistics
using CircuitStatistics = ::std::unordered_map<::std::string, double>;

/**
 * @brief Structure defining a quantum gate operation
 */
struct GateOperation {
    std::size_t target;
    std::size_t control;
    GateMatrix matrix;
    std::string name;
    bool isControlled{false};
};

/**
 * @brief Structure containing quantum measurement results
 */
struct QuantumMeasurement {
    std::size_t outcome{0};                         ///< Measurement outcome
    std::vector<double> probabilities{};            ///< Measurement probabilities
    double fidelity{0.0};                          ///< Measurement fidelity
    std::vector<double> amplitudes{};              ///< State amplitudes
    std::vector<std::size_t> measured_qubits{};    ///< Measured qubit indices
};

/**
 * @brief Structure for quantum error correction data
 */
struct ErrorSyndrome {
    std::vector<std::size_t> error_qubits{};       ///< Qubits with errors
    std::vector<GateType> correction_gates{};       ///< Gates needed for correction
    double error_rate{0.0};                        ///< Error rate
    bool requires_recovery{false};                 ///< Whether recovery is needed
};

/**
 * @brief Structure containing quantum security metrics
 */
struct QuantumSecurityMetrics {
    double entanglement{0.0};                      ///< Entanglement measure
    double coherence{0.0};                         ///< Coherence measure
    double error_rate{0.0};                        ///< Error rate
    double fidelity{0.0};                          ///< State fidelity
    std::size_t circuit_depth{0};                  ///< Circuit depth
    std::size_t num_qubits{0};                     ///< Number of qubits
};

/// Constants for quantum operations
namespace constants {
    constexpr double QUANTUM_ERROR_THRESHOLD = 1e-6;     ///< Error threshold for quantum operations
    constexpr double QUANTUM_ENTANGLEMENT_THRESHOLD = 1e-5;
    constexpr std::size_t DEFAULT_QUBIT_COUNT = 8;       ///< Default number of qubits
    constexpr std::size_t MAX_QUBIT_COUNT = 32;
    constexpr std::size_t DEFAULT_CIRCUIT_DEPTH = 4;
    constexpr std::size_t MAX_CIRCUIT_DEPTH = 1000;
    constexpr double DEFAULT_LEARNING_RATE = 0.001;
    constexpr double MIN_FIDELITY = 0.99;
}

/// Common quantum gates as constant matrices
namespace gates {
    /// Hadamard gate matrix
    const OperatorMatrix HADAMARD = (OperatorMatrix(2,2) << 
        1/std::sqrt(2), 1/std::sqrt(2),
        1/std::sqrt(2), -1/std::sqrt(2)).finished();

    /// Pauli-X (NOT) gate matrix
    const OperatorMatrix PAULI_X = (OperatorMatrix(2,2) << 
        0, 1,
        1, 0).finished();

    /// Pauli-Y gate matrix
    const OperatorMatrix PAULI_Y = (OperatorMatrix(2,2) << 
        0, Complex(0,-1),
        Complex(0,1), 0).finished();

    /// Pauli-Z gate matrix
    const OperatorMatrix PAULI_Z = (OperatorMatrix(2,2) << 
        1, 0,
        0, -1).finished();
}

/**
 * @brief Types of quantum errors that can occur
 */
enum class ErrorCode {
    NONE,        ///< No error
    BIT_FLIP,    ///< Bit flip error
    PHASE_FLIP,  ///< Phase flip error
    COMBINED     ///< Combined bit and phase flip
};

/**
 * @brief Measurement basis options
 */
enum class Basis {
    COMPUTATIONAL,  ///< Standard computational basis
    HADAMARD,      ///< Hadamard basis
    BELL           ///< Bell state basis
};

// Forward declarations
class QuantumState;
class QuantumCircuit;
class QuantumGate;

} // namespace quids::quantum

#endif // QUIDS_QUANTUM_TYPES_HPP
