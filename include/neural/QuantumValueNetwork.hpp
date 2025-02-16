#ifndef QUIDS_NEURAL_QUANTUM_VALUE_NETWORK_HPP
#define QUIDS_NEURAL_QUANTUM_VALUE_NETWORK_HPP

#include "BaseQuantumNetwork.hpp"
#include "quantum/QuantumState.hpp"
#include <vector>
#include <memory>

namespace quids::neural {

class QuantumValueNetwork : public ::quids::neural::BaseQuantumNetwork {
public:
    QuantumValueNetwork(std::size_t stateSize, std::size_t numQubits);
    ~QuantumValueNetwork() override = default;

    // Disable copy operations
    QuantumValueNetwork(const QuantumValueNetwork&) = delete;
    QuantumValueNetwork& operator=(const QuantumValueNetwork&) = delete;

    // Enable move operations
    QuantumValueNetwork(QuantumValueNetwork&&) noexcept = default;
    QuantumValueNetwork& operator=(QuantumValueNetwork&&) noexcept = default;

    // Parameter access (from BaseQuantumNetwork)
    double getParameter(std::size_t index) const override;
    void setParameter(std::size_t index, double value) override;
    std::size_t getNumParameters() const override;

    // Network operations (from BaseQuantumNetwork)
    void forward() override;
    void backward() override;
    std::vector<double> getGradients() const override;

    // Value-specific operations
    std::vector<double> forward(const ::quids::quantum::QuantumState& state);
    void updateValue(const std::vector<double>& targets);
    double getValueLoss() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace quids::neural 

#endif // QUIDS_NEURAL_QUANTUM_VALUE_NETWORK_HPP 