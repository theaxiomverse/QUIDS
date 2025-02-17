#ifndef QUIDS_NEURAL_QUANTUM_VALUE_NETWORK_HPP
#define QUIDS_NEURAL_QUANTUM_VALUE_NETWORK_HPP

#include "neural/BaseQuantumNetwork.hpp"
#include "quantum/QuantumState.hpp"
#include <memory>
#include <string>
#include <vector>

namespace quids::neural {

class QuantumValueNetwork : public BaseQuantumNetwork {
public:
    QuantumValueNetwork();
    ~QuantumValueNetwork() override;

    // Implement pure virtual methods from BaseQuantumNetwork
    [[nodiscard]] double getParameter(::std::size_t index) const override;
    void setParameter(::std::size_t index, double value) override;
    [[nodiscard]] ::std::size_t getNumParameters() const override;
    [[nodiscard]] ::std::vector<double> getGradients() const override;
    [[nodiscard]] ::std::vector<double> getQuantumParameters() const override;
    void forward() override;
    void backward() override;
    void loadNetworkState(const ::std::string& filePath) override;
    void saveNetworkState(const ::std::string& filePath) const override;
    [[nodiscard]] ::std::vector<double> calculateQuantumGradients() const override;

    // Value network specific methods
    [[nodiscard]] double getValue(const quantum::QuantumState& state) const;
    void updateValue(const quantum::QuantumState& state, double target);
    [[nodiscard]] double getValueLoss() const;

private:
    class Impl;
    ::std::unique_ptr<Impl> impl_;
};

} // namespace quids::neural

#endif // QUIDS_NEURAL_QUANTUM_VALUE_NETWORK_HPP 