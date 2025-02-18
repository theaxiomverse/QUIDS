#ifndef QUIDS_QUANTUM_CIRCUIT_CONFIG_HPP
#define QUIDS_QUANTUM_CIRCUIT_CONFIG_HPP

#include <cstddef>
#include <vector>
#include <string>

namespace quids::quantum {

/**
 * @brief Configuration for quantum circuits
 */
struct QuantumCircuitConfig {
    size_t numQubits{8};
    size_t depth{4};
    bool useErrorCorrection{true};
    bool useParallelExecution{true};
    double errorThreshold{0.001};
    std::vector<std::string> customGates;
    std::vector<double> gateParameters;
};

} // namespace quids::quantum

#endif // QUIDS_QUANTUM_CIRCUIT_CONFIG_HPP