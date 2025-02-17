#ifndef QUIDS_QUANTUM_QUANTUM_FORWARD_HPP
#define QUIDS_QUANTUM_QUANTUM_FORWARD_HPP

/**
 * @file QuantumForward.hpp
 * @brief Forward declarations for quantum computing module
 * 
 * This file contains forward declarations of classes, structs, and enums
 * used throughout the quantum computing module. It helps break circular
 * dependencies and reduces compilation time.
 */

namespace quids::quantum {

// Forward declarations of classes
class QuantumState;
class QuantumCircuit;
class QuantumRegister;
class QuantumGate;

// Forward declarations of structs
struct QuantumMeasurement;
struct QuantumCircuitConfig;
struct GateOperation;
struct ErrorSyndrome;
struct QuantumSecurityMetrics;

// Forward declarations of enums
enum class GateType;
enum class ErrorCode;
enum class Basis;

} // namespace quids::quantum

#endif // QUIDS_QUANTUM_QUANTUM_FORWARD_HPP 