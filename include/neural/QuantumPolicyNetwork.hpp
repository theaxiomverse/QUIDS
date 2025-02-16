#ifndef QUIDS_NEURAL_QUANTUM_POLICY_NETWORK_HPP
#define QUIDS_NEURAL_QUANTUM_POLICY_NETWORK_HPP

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
    QuantumPolicyNetwork(std::size_t stateSize, std::size_t actionSize, std::size_t numQubits);
    ~QuantumPolicyNetwork() override;

    // Disable copy operations
    QuantumPolicyNetwork(const QuantumPolicyNetwork&) = delete;
    QuantumPolicyNetwork& operator=(const QuantumPolicyNetwork&) = delete;

    // Enable move operations
    QuantumPolicyNetwork(QuantumPolicyNetwork&&) noexcept = default;
    QuantumPolicyNetwork& operator=(QuantumPolicyNetwork&&) noexcept = default;

    // Parameter access (from BaseQuantumNetwork)
    double getParameter(std::size_t index) const override;
    void setParameter(std::size_t index, double value) override;
    std::size_t getNumParameters() const override;

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

#endif // QUIDS_NEURAL_QUANTUM_POLICY_NETWORK_HPP 