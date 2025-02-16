#pragma once

#include "BaseQuantumNetwork.hpp"
#include "quantum/QuantumState.hpp"
#include <vector>
#include <memory>

namespace quids::neural {

class QuantumValueNetwork : public BaseQuantumNetwork {
public:
    QuantumValueNetwork(size_t stateSize, size_t numQubits);
    ~QuantumValueNetwork() override = default;

    // Parameter access (from BaseQuantumNetwork)
    double getParameter(size_t index) const override;
    void setParameter(size_t index, double value) override;
    size_t getNumParameters() const override;

    // Network operations (from BaseQuantumNetwork)
    void forward() override;
    void backward() override;
    std::vector<double> getGradients() const override;

    // Value-specific operations
    std::vector<double> forward(const quantum::QuantumState& state);
    void updateValue(const std::vector<double>& targets);
    double getValueLoss() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace quids::neural 