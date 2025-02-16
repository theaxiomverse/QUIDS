#pragma once

#include "neural/QuantumPolicyNetwork.hpp"
#include "quantum/QuantumState.hpp"
#include <vector>

namespace quids::neural {

struct QuantumPolicyNetwork::Impl {
    size_t stateSize;
    size_t actionSize;
    size_t numQubits;
    std::vector<double> parameters;
    std::vector<double> gradients;
    double entropy{0.0};
    quantum::QuantumState currentState;

    Impl(size_t s, size_t a, size_t n) 
        : stateSize(s), actionSize(a), numQubits(n),
          parameters(s * a), gradients(s * a), currentState(n) {
        initializeParameters();
    }
    void initializeParameters();
};

} // namespace quids::neural 