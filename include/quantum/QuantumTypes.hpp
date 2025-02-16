#pragma once

#include <Eigen/Dense>
#include <cstdint>
#include <vector>
#include <complex>
#include <unordered_map>
#include <iostream>
#include <functional>
#include "QuantumForward.hpp"

namespace quids {
namespace quantum {

using Complex = std::complex<double>;

// Forward declarations
class QuantumState;

// Gate types for quantum operations
enum class GateType {
    HADAMARD,
    PAULI_X,
    PAULI_Y,
    PAULI_Z,
    CNOT,
    SWAP,
    TOFFOLI,
    PHASE,
    ROTATION,
    CUSTOM
};

// Structure for quantum gate operations
struct GateOperation {
    GateType type{GateType::HADAMARD};
    std::vector<size_t> qubits{};
    std::vector<double> parameters{};
    GateMatrix custom_matrix{};
};

// Quantum measurement results
struct QuantumMeasurement {
    size_t outcome{0};
    std::vector<double> probabilities{};
    double fidelity{0.0};
    std::vector<double> amplitudes{};
    std::vector<size_t> measured_qubits{};
};

// Quantum error correction data
struct ErrorSyndrome {
    std::vector<size_t> error_qubits{};
    std::vector<GateType> correction_gates{};
    double error_rate{0.0};
    bool requires_recovery{false};
};

// Quantum security metrics
struct QuantumSecurityMetrics {
    double entanglement{0.0};
    double coherence{0.0};
    double error_rate{0.0};
    double fidelity{0.0};
    size_t circuit_depth{0};
    size_t num_qubits{0};
};



// Constants for quantum operations
constexpr double QUANTUM_ERROR_THRESHOLD = 1e-6;
constexpr size_t DEFAULT_QUBIT_COUNT = 8;
constexpr size_t MAX_ENTANGLEMENT_DEPTH = 100;

} // namespace quantum
} // namespace quids
