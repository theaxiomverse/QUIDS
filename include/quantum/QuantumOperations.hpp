#pragma once


#include "quantum/QuantumState.hpp"

#include <Eigen/Dense>
#include "quantum/QuantumTypes.hpp"
#include <vector>
#include <cstdint>

namespace quids {
namespace quantum {

class QuantumState;

// Apply a controlled quantum operation with arbitrary gate matrix
void apply_controlled_operation(quids::quantum::QuantumState& state,
                            size_t control,
                            size_t target,
                            const Eigen::Matrix2cd& operation);

// Apply Toffoli gate (controlled-controlled-NOT)
void apply_toffoli(quids::quantum::QuantumState& state,
                 size_t control1,
                 size_t control2,
                 size_t target);

// Apply SWAP gate between two qubits
void apply_swap(quids::quantum::QuantumState& state,
               size_t qubit1,
               size_t qubit2);

} // namespace quantum
} // namespace quids


namespace quids {
namespace quantum {
namespace detail {

// Convert classical bits to quantum state
quids::quantum::QuantumState classical_to_quantum(const std::vector<uint8_t>& classical_data);

// Calculate quantum state properties
double calculate_entanglement(const quids::quantum::QuantumState& state);
double calculate_coherence(const quids::quantum::QuantumState& state);
double calculate_fidelity(const quids::quantum::QuantumState& state1, const quids::quantum::QuantumState& state2);

// Error correction utilities
quids::quantum::QuantumState detect_errors(const quids::quantum::QuantumState& state);
quids::quantum::QuantumState correct_errors(const quids::quantum::QuantumState& state, const quids::quantum::QuantumState& syndrome);

} // namespace detail
} // namespace quantum
} // namespace quids 