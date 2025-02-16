#pragma once

#include "BaseQuantumNetwork.hpp"
#include <vector>
#include <memory>

namespace quids::quantum {
    class QuantumState;
}

namespace quids::neural {

namespace detail {
    struct QuantumPolicyNetworkState;
}

class QuantumPolicyNetwork : public BaseQuantumNetwork {
public:
    QuantumPolicyNetwork(size_t stateSize, size_t actionSize, size_t numQubits);
    ~QuantumPolicyNetwork() override;

    // Parameter access (from BaseQuantumNetwork)
    double getParameter(size_t index) const override;
    void setParameter(size_t index, double value) override;
    size_t getNumParameters() const override;

    // Network operations (from BaseQuantumNetwork)
    void forward() override;
    void backward() override;
    std::vector<double> getGradients() const override;

    // Policy-specific operations
    std::vector<double> forward(const quantum::QuantumState& state);
    void updatePolicy(const std::vector<double>& advantages);
    double getPolicyEntropy() const;

private:
    struct Impl;  // Forward declare the implementation struct
    std::unique_ptr<Impl> impl_;
};

} // namespace quids::neural 