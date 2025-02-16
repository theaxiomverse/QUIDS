#include "neural/QuantumValueNetwork.hpp"
#include "quantum/QuantumCircuit.hpp"
#include <Eigen/Dense>
#include <random>
#include <stdexcept>

namespace quids::neural {

class QuantumValueNetwork::Impl {
public:
    Impl(std::size_t stateSize, std::size_t numQubits)
        : stateSize_(stateSize),
          numQubits_(numQubits),
          parameters_(stateSize),
          gradients_(stateSize),
          valueLoss_(0.0) {
        initializeParameters();
    }

    void initializeParameters() noexcept {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> d(0.0, 0.1);
        
        for (auto& param : parameters_) {
            param = d(gen);
        }
    }

    std::size_t stateSize_;
    std::size_t numQubits_;
    std::vector<double> parameters_;
    std::vector<double> gradients_;
    double valueLoss_;
    quantum::QuantumState currentState_;
};

QuantumValueNetwork::QuantumValueNetwork(std::size_t stateSize, std::size_t numQubits)
    : impl_(std::make_unique<Impl>(stateSize, numQubits)) {}

double QuantumValueNetwork::getParameter(std::size_t index) const {
    if (index >= impl_->parameters_.size()) {
        throw std::out_of_range("Parameter index out of range");
    }
    return impl_->parameters_[index];
}

void QuantumValueNetwork::setParameter(std::size_t index, double value) {
    if (index >= impl_->parameters_.size()) {
        throw std::out_of_range("Parameter index out of range");
    }
    impl_->parameters_[index] = value;
}

size_t QuantumValueNetwork::getNumParameters() const {
    return impl_->parameters_.size();
}

void QuantumValueNetwork::forward() {
    // Base forward pass without state input
    // This is used for internal network operations
}

void QuantumValueNetwork::backward() {
    // Compute gradients for all parameters
    // This is called during training
}

std::vector<double> QuantumValueNetwork::getGradients() const {
    return impl_->gradients_;
}

std::vector<double> QuantumValueNetwork::forward(const quantum::QuantumState& state) {
    impl_->currentState_ = state;
    
    // Apply quantum circuit to get state value
    Eigen::VectorXd stateVector = state.getStateVector().real();
    Eigen::VectorXd valueVector = Eigen::Map<Eigen::VectorXd>(impl_->parameters_.data(), impl_->parameters_.size());
    
    // Compute value using dot product
    double value = stateVector.dot(valueVector);
    
    return {value};
}

void QuantumValueNetwork::updateValue(const std::vector<double>& targets) {
    if (targets.empty()) {
        throw std::invalid_argument("Targets vector is empty");
    }
    
    // Get current value
    double currentValue = forward(impl_->currentState_)[0];
    double target = targets[0];
    
    // Compute MSE loss
    impl_->valueLoss_ = 0.5 * std::pow(target - currentValue, 2);
    
    // Compute value gradients
    Eigen::VectorXd stateVector = impl_->currentState_.getStateVector().real();
    double delta = currentValue - target;
    
    for (size_t i = 0; i < impl_->stateSize_; ++i) {
        impl_->gradients_[i] = delta * stateVector(i);
    }
}

double QuantumValueNetwork::getValueLoss() const {
    return impl_->valueLoss_;
}

} // namespace quids::neural 