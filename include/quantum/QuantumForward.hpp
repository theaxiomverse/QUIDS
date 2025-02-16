#pragma once

#include <Eigen/Dense>
#include <complex>

namespace quids {
namespace quantum {

using Complex = std::complex<double>;
using StateVector = Eigen::VectorXcd;
using GateMatrix = Eigen::Matrix<Complex, Eigen::Dynamic, Eigen::Dynamic>;

// Forward declarations
class QuantumState;
class QuantumCircuit;

// Forward declarations of structs
struct QuantumCircuitConfig;
struct QuantumMeasurement;
struct GateOperation;

// Forward declarations of enums
enum class GateType;

} // namespace quantum
} // namespace quids 