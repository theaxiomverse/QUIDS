#include "neural/QuantumPolicyNetwork.hpp"
#include "quantum/QuantumState.hpp"
#include "quantum/QuantumCircuit.hpp"
#include <Eigen/Dense>
#include <random>
#include <cmath>
#include <stdexcept>

namespace quids::neural {
namespace detail {

struct QuantumPolicyNetworkState {
    size_t stateSize;
    size_t actionSize;
    size_t numQubits;
    std::vector<double> parameters;
    std::vector<double> gradients;
    double entropy{0.0};
    quantum::QuantumState currentState;

    QuantumPolicyNetworkState(size_t s, size_t a, size_t n);
    void initializeParameters();
};

QuantumPolicyNetworkState::QuantumPolicyNetworkState(size_t s, size_t a, size_t n)
    : stateSize(s), actionSize(a), numQubits(n),
      parameters(s * a), gradients(s * a), currentState(n) {
    initializeParameters();
}

void QuantumPolicyNetworkState::initializeParameters() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dist(0.0, 0.1);
    for (auto& param : parameters) {
        param = dist(gen);
    }
}

} // namespace detail

QuantumPolicyNetwork::QuantumPolicyNetwork(size_t stateSize, size_t actionSize, size_t numQubits)
    : impl_(std::make_unique<detail::QuantumPolicyNetworkState>(stateSize, actionSize, numQubits)) {}

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

void QuantumPolicyNetwork::forward() {
    // Base forward pass without state input
    // This is used for internal network operations
}

void QuantumPolicyNetwork::backward() {
    // Compute gradients for all parameters
    // This is called during training
}

std::vector<double> QuantumPolicyNetwork::getGradients() const {
    return impl_->gradients;
}

std::vector<double> QuantumPolicyNetwork::forward(const quantum::QuantumState& state) {
    impl_->currentState = state;
    std::vector<double> actionProbs(impl_->actionSize);
    
    // Apply quantum circuit to get action probabilities
    Eigen::VectorXd stateVector = state.getStateVector().real();
    Eigen::MatrixXd policyMatrix(impl_->stateSize, impl_->actionSize);
    
    // Reshape parameters into policy matrix
    const auto& params = impl_->parameters;
    for (size_t i = 0; i < impl_->stateSize; ++i) {
        for (size_t j = 0; j < impl_->actionSize; ++j) {
            policyMatrix(i, j) = params[i * impl_->actionSize + j];
        }
    }
    
    // Compute action probabilities using softmax
    Eigen::VectorXd logits = policyMatrix.transpose() * stateVector;
    double maxLogit = logits.maxCoeff();
    Eigen::VectorXd expLogits = (logits.array() - maxLogit).exp();
    double sumExpLogits = expLogits.sum();
    
    for (size_t i = 0; i < impl_->actionSize; ++i) {
        actionProbs[i] = expLogits(i) / sumExpLogits;
    }
    
    // Compute entropy for regularization
    double entropy = 0.0;
    for (double prob : actionProbs) {
        if (prob > 0.0) {
            entropy -= prob * std::log(prob);
        }
    }
    
    return actionProbs;
}

void QuantumPolicyNetwork::updatePolicy(const std::vector<double>& advantages) {
    if (advantages.size() != impl_->actionSize) {
        throw std::invalid_argument("Advantages size does not match action size");
    }
    
    // Compute policy gradients
    Eigen::VectorXd stateVector = impl_->currentState.getStateVector().real();
    std::vector<double> gradients(impl_->parameters.size());
    
    for (size_t i = 0; i < impl_->stateSize; ++i) {
        for (size_t j = 0; j < impl_->actionSize; ++j) {
            size_t paramIndex = i * impl_->actionSize + j;
            gradients[paramIndex] = stateVector(i) * advantages[j];
        }
    }
    
    impl_->gradients = gradients;
}

double QuantumPolicyNetwork::getPolicyEntropy() const {
    return impl_->entropy;
}

} // namespace quids::neural 