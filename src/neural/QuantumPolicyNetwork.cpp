#include "neural/QuantumPolicyNetwork.hpp"
#include <Eigen/Dense>
#include <cmath>
#include <random>

namespace quids::neural {

struct QuantumPolicyNetwork::Impl {
    Impl(size_t stateSize, size_t actionSize, size_t numQubits)
        : stateSize(stateSize), actionSize(actionSize), numQubits(numQubits),
          parameters(stateSize * actionSize), gradients(stateSize * actionSize),
          policyMatrix(Eigen::MatrixXd::Zero(stateSize, actionSize)) {}

    size_t stateSize;
    size_t actionSize;
    size_t numQubits;
    std::vector<double> parameters;
    std::vector<double> gradients;
    Eigen::MatrixXd policyMatrix;
    std::vector<double> currentState;
};

QuantumPolicyNetwork::QuantumPolicyNetwork(size_t stateSize, size_t actionSize, size_t numQubits)
    : impl_(std::make_unique<Impl>(stateSize, actionSize, numQubits)) {}

QuantumPolicyNetwork::~QuantumPolicyNetwork() = default;

double QuantumPolicyNetwork::getParameter(size_t index) const {
    if (index >= impl_->parameters.size()) {
        throw std::out_of_range("Parameter index out of range");
    }
    return impl_->parameters[index];
}

void QuantumPolicyNetwork::setParameter(size_t index, double value) {
    if (index >= impl_->parameters.size()) {
        throw std::out_of_range("Parameter index out of range");
    }
    impl_->parameters[index] = value;
}

size_t QuantumPolicyNetwork::getNumParameters() const {
    return impl_->parameters.size();
}

std::vector<double> QuantumPolicyNetwork::getGradients() const {
    return impl_->gradients;
}

std::vector<double> QuantumPolicyNetwork::getQuantumParameters() const {
    return impl_->parameters;
}

void QuantumPolicyNetwork::forward(const std::vector<double>& state) {
    if (state.empty()) {
        throw std::runtime_error("State vector is empty");
    }

    impl_->currentState = state;

    // Reshape parameters into policy matrix
    for (size_t i = 0; i < impl_->stateSize; ++i) {
        for (size_t j = 0; j < impl_->actionSize; ++j) {
            impl_->policyMatrix(i, j) = impl_->parameters[i * impl_->actionSize + j];
        }
    }

    // Apply softmax to get action probabilities
    for (size_t i = 0; i < impl_->stateSize; ++i) {
        double max_val = impl_->policyMatrix.row(i).maxCoeff();
        impl_->policyMatrix.row(i) = (impl_->policyMatrix.row(i).array() - max_val).exp();
        impl_->policyMatrix.row(i) /= impl_->policyMatrix.row(i).sum();
    }
}

void QuantumPolicyNetwork::backward(const std::vector<double>& gradOutput) {
    if (gradOutput.size() != impl_->actionSize) {
        throw std::runtime_error("Invalid gradient output size");
    }

    // Compute gradients
    for (size_t i = 0; i < impl_->stateSize; ++i) {
        for (size_t j = 0; j < impl_->actionSize; ++j) {
            size_t idx = i * impl_->actionSize + j;
            impl_->gradients[idx] = gradOutput[j] * impl_->policyMatrix(i, j) * 
                                  (1.0 - impl_->policyMatrix(i, j));
        }
    }
}

void QuantumPolicyNetwork::updatePolicy(const std::vector<double>& rewards) {
    if (rewards.empty()) {
        throw std::runtime_error("Rewards vector is empty");
    }

    // Update parameters using rewards and gradients
    double learning_rate = 0.01; // Could be made configurable
    for (size_t i = 0; i < impl_->parameters.size(); ++i) {
        impl_->parameters[i] += learning_rate * impl_->gradients[i] * rewards[0];
    }
}

std::vector<double> QuantumPolicyNetwork::getActionProbabilities() const {
    if (impl_->currentState.empty()) {
        throw std::runtime_error("No state has been processed yet");
    }

    std::vector<double> probs(impl_->actionSize);
    for (size_t i = 0; i < impl_->actionSize; ++i) {
        probs[i] = impl_->policyMatrix(0, i); // Using first row as current state
    }
    return probs;
}

void QuantumPolicyNetwork::resetNetworkState() {
    impl_->parameters = std::vector<double>(impl_->stateSize * impl_->actionSize);
    impl_->gradients = std::vector<double>(impl_->stateSize * impl_->actionSize);
    impl_->policyMatrix = Eigen::MatrixXd::Zero(impl_->stateSize, impl_->actionSize);
    impl_->currentState.clear();
}

} // namespace quids::neural 