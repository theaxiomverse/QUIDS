#pragma once
#include <vector>
#include "QuantumTypes.hpp"



namespace quids {
namespace quantum {


struct QuantumCircuitConfig {
    // Configuration parameters
    size_t num_qubits{1};
    size_t max_depth{1};
    double error_rate{0.0};
    bool use_error_correction{false};
   std::vector<GateMatrix> allowed_gates{};
    size_t num_measurements{0};
};


} // namespace quantum
} // namespace quids