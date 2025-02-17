#pragma once
#include "quantum/QuantumOperations.hpp"

#include <stdexcept>
#include <complex>
#include <cmath>

namespace quids {
namespace quantum {
namespace operations {




// Implementation of quantum operations
void applyControlledOperation(QuantumState& state, 
                            size_t control, 
                            size_t target,
                            const Eigen::Matrix2cd& operation) {
    if (control >= state.get_num_qubits() || target >= state.get_num_qubits()) {
        throw std::out_of_range("Qubit indices out of range");
    }

    size_t dim = 1ULL << state.get_num_qubits();
    size_t control_mask = 1ULL << control;
    size_t target_mask = 1ULL << target;

    for (size_t i = 0; i < dim; i++) {
        if ((i & control_mask) != 0) {  // Control qubit is |1⟩
            size_t basis_state = i;
            size_t paired_state = i ^ target_mask;
            
            std::complex<double> alpha = state.getAmplitude(basis_state);
            std::complex<double> beta = state.getAmplitude(paired_state);
            
            state.setAmplitude(basis_state, 
                operation(0, 0) * alpha + operation(0, 1) * beta);
            state.setAmplitude(paired_state, 
                operation(1, 0) * alpha + operation(1, 1) * beta);
        }
    }
}

void applyToffoli(QuantumState& state, 
                 size_t control1, 
                 size_t control2, 
                 size_t target) {
    if (control1 >= state.get_num_qubits() || 
        control2 >= state.get_num_qubits() || 
        target >= state.get_num_qubits()) {
        throw std::out_of_range("Qubit indices out of range");
    }

    size_t dim = 1ULL << state.get_num_qubits();
    size_t control1_mask = 1ULL << control1;
    size_t control2_mask = 1ULL << control2;
    size_t target_mask = 1ULL << target;

    for (size_t i = 0; i < dim; i++) {
        if ((i & control1_mask) != 0 && (i & control2_mask) != 0) {
            size_t paired_state = i ^ target_mask;
            std::complex<double> temp = state.getAmplitude(i);
            state.setAmplitude(i, state.getAmplitude(paired_state));
            state.setAmplitude(paired_state, temp);
        }
    }
}

void applySwap(QuantumState& state, size_t qubit1, size_t qubit2) {
    if (qubit1 >= state.get_num_qubits() || qubit2 >= state.get_num_qubits()) {
        throw std::out_of_range("Qubit indices out of range");
    }

    size_t dim = 1ULL << state.get_num_qubits();
    size_t mask1 = 1ULL << qubit1;
    size_t mask2 = 1ULL << qubit2;

    for (size_t i = 0; i < dim; i++) {
        if ((i & mask1) != (i & mask2)) {
            size_t paired_state = i ^ mask1 ^ mask2;
            if (i < paired_state) {
                std::complex<double> temp = state.getAmplitude(i);
                state.setAmplitude(i, state.getAmplitude(paired_state));
                state.setAmplitude(paired_state, temp);
            }
        }
    }
}

} // namespace quantum
} // namespace quids
}