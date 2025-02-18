#ifndef QUIDS_NEURAL_QUANTUM_POLICY_NETWORK_HPP
#define QUIDS_NEURAL_QUANTUM_POLICY_NETWORK_HPP

#include "BaseQuantumNetwork.hpp"
#include <vector>
#include <memory>

namespace quids::neural {

class QuantumPolicyNetwork : public BaseQuantumNetwork {
public:
    explicit QuantumPolicyNetwork(size_t inputSize, size_t outputSize, size_t numQubits);
    ~QuantumPolicyNetwork() override;

    // Delete copy operations
    QuantumPolicyNetwork(const QuantumPolicyNetwork&) = delete;
    QuantumPolicyNetwork& operator=(const QuantumPolicyNetwork&) = delete;

    // Delete move operations
    QuantumPolicyNetwork(QuantumPolicyNetwork&&) = delete;
    QuantumPolicyNetwork& operator=(QuantumPolicyNetwork&&) = delete;

    // Parameter access
    [[nodiscard]] double getParameter(size_t index) const override;
    void setParameter(size_t index, double value) override;

    // Network operations
    void forward() override;
    void backward() override;
    [[nodiscard]] std::vector<double> calculateQuantumGradients() const override;
    void loadNetworkState(const std::string& filePath) override;
    void saveNetworkState(const std::string& filePath) const override;

    // Policy-specific operations
    void forward(const std::vector<double>& state);
    void backward(const std::vector<double>& gradOutput);
    void updatePolicy(const std::vector<double>& prediction);
    [[nodiscard]] double getPolicyEntropy() const;
    [[nodiscard]] std::vector<double> getQuantumParameters() const override;
    [[nodiscard]] size_t getNumParameters() const override;
    [[nodiscard]] std::vector<double> getGradients() const override;
    [[nodiscard]] std::vector<double> getPolicy() const;
    [[nodiscard]] std::vector<double> getCurrentState() const;
    [[nodiscard]] std::vector<double> getActionProbabilities() const;

    void updatePolicy(const std::vector<double>& rewards, double learningRate);
    void updatePolicy(const std::vector<double>& rewards, double learningRate, double discountFactor);
    void resetNetworkState();
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace quids::neural

#endif // QUIDS_NEURAL_QUANTUM_POLICY_NETWORK_HPP 