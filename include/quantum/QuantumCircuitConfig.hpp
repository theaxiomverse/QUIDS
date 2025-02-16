#ifndef QUIDS_QUANTUM_QUANTUM_CIRCUIT_CONFIG_HPP
#define QUIDS_QUANTUM_QUANTUM_CIRCUIT_CONFIG_HPP

#include "QuantumTypes.hpp"
#include <cstdint>

namespace quids::quantum {

/**
 * @brief Configuration options for quantum circuits
 * 
 * This structure contains parameters that control the behavior and
 * optimization of quantum circuits, including resource limits and
 * optimization settings.
 */
struct QuantumCircuitConfig {
    std::size_t num_qubits{constants::DEFAULT_QUBIT_COUNT};  ///< Number of qubits in circuit
    std::size_t max_depth{1000};                             ///< Maximum circuit depth
    std::size_t max_gates{10000};                           ///< Maximum number of gates
    double error_threshold{constants::QUANTUM_ERROR_THRESHOLD}; ///< Error tolerance threshold
    bool optimize_gates{true};                               ///< Whether to optimize gates
    bool verify_unitarity{true};                            ///< Whether to verify gate unitarity
    bool enable_parallelization{true};                      ///< Whether to enable parallel execution
    bool store_intermediate_states{false};                  ///< Whether to store intermediate states

    /**
     * @brief Validates the configuration
     * @return true if configuration is valid, false otherwise
     */
    [[nodiscard]] bool isValid() const noexcept {
        return num_qubits > 0 && 
               max_depth > 0 && 
               max_gates > 0 && 
               error_threshold > 0.0;
    }
};

} // namespace quids::quantum

#endif // QUIDS_QUANTUM_QUANTUM_CIRCUIT_CONFIG_HPP