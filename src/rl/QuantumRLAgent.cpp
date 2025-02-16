#include "rl/QuantumRLAgent.hpp"
#include "neural/QuantumOptimizer.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <random>

namespace quids::rl {

QuantumRLAgent::QuantumRLAgent(const QuantumRLConfig& config)
    : config_(config) {
    // Initialize neural networks
    policyNet_ = std::make_unique<neural::QuantumPolicyNetwork>(config.stateSize, config.actionSize, config.numQubits);
    valueNet_ = std::make_unique<neural::QuantumValueNetwork>(config.stateSize, config.numQubits);
    
    // Initialize quantum circuit
    circuit_ = std::make_unique<quantum::QuantumCircuit>(config.circuitConfig);
    
    // Reserve space for replay buffer
    replayBuffer_.reserve(config.replayBufferSize);
}

Action QuantumRLAgent::decideActionQuantum(const quantum::QuantumState& state) {
    // Prepare quantum state from input state
    auto quantumState = prepareQuantumState(state);
    
    // Get action probabilities from policy network
    auto actionProbs = policyNet_->forward(quantumState);
    
    // Epsilon-greedy exploration
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    if (dis(gen) < config_.explorationRate) {
        // Random action
        std::uniform_int_distribution<> actionDis(0, config_.actionSize - 1);
        return Action{actionDis(gen)};
    } else {
        // Greedy action
        return Action{static_cast<size_t>(std::max_element(actionProbs.begin(), actionProbs.end()) - actionProbs.begin())};
    }
}

void QuantumRLAgent::train(const std::vector<Experience>& experiences) {
    // Add experiences to replay buffer
    for (const auto& exp : experiences) {
        updateReplayBuffer(exp);
    }
    
    // Only train if we have enough experiences
    if (replayBuffer_.size() < MIN_EXPERIENCES_FOR_TRAINING) {
        return;
    }
    
    // Sample random batch
    std::vector<Experience> batch;
    batch.reserve(config_.batchSize);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, replayBuffer_.size() - 1);
    
    for (size_t i = 0; i < config_.batchSize; ++i) {
        batch.push_back(replayBuffer_[dis(gen)]);
    }
    
    // Process batch using SIMD
    processExperiencesBatch(batch);
    
    // Update networks
    updateNetworksSIMD();
    
    // Update metrics
    metrics_.trainingSteps++;
}

void QuantumRLAgent::updateQuantumPolicy() {
    optimizePolicyQuantum();
    optimizeValueQuantum();
}

QuantumRLConfig QuantumRLAgent::getConfig() const {
    return config_;
}

const QuantumRLAgent::Metrics& QuantumRLAgent::getMetrics() const {
    return metrics_;
}

void QuantumRLAgent::setExplorationRate(double rate) {
    config_.explorationRate = std::clamp(rate, 0.0, 1.0);
}

quantum::QuantumState QuantumRLAgent::prepareQuantumState(const State& classicalState) {
    // Convert classical state to quantum state using the quantum circuit
    return circuit_->prepareState(classicalState);
}

Action QuantumRLAgent::measureQuantumState(const quantum::QuantumState& state) {
    // Measure quantum state to get classical action
    auto measurement = circuit_->measure(state);
    return Action{measurement.outcome};
}

double QuantumRLAgent::calculateQuantumAdvantage(const quantum::QuantumState& state, const Action& action) {
    // Calculate quantum advantage using value network
    return valueNet_->forward(state)[action.index];
}

void QuantumRLAgent::updateReplayBuffer(const Experience& experience) {
    if (replayBuffer_.size() >= config_.replayBufferSize) {
        // Remove oldest experience
        replayBuffer_.erase(replayBuffer_.begin());
    }
    replayBuffer_.push_back(experience);
}

void QuantumRLAgent::optimizePolicyQuantum() {
    // Optimize policy network using quantum gradients
    auto gradients = policyNet_->getGradients();
    neural::QuantumOptimizer optimizer(config_.learningRate);
    optimizer.applyQuantumGradients(policyNet_.get(), gradients);
}

void QuantumRLAgent::optimizeValueQuantum() {
    // Optimize value network using quantum gradients
    auto gradients = valueNet_->getGradients();
    neural::QuantumOptimizer optimizer(config_.learningRate);
    optimizer.applyQuantumGradients(valueNet_.get(), gradients);
}

void QuantumRLAgent::processExperiencesBatch(const std::vector<Experience>& batch) {
    // Process experiences in parallel using SIMD
    #pragma omp simd
    for (size_t i = 0; i < batch.size(); i += SIMD_WIDTH) {
        // Process SIMD_WIDTH experiences at once
        auto states = prepareQuantumState(batch[i].state);
        auto nextStates = prepareQuantumState(batch[i].nextState);
        
        // Calculate advantages and update networks
        auto advantage = calculateQuantumAdvantage(states, batch[i].action);
        metrics_.averageReward = 0.9 * metrics_.averageReward + 0.1 * batch[i].reward;
    }
}

void QuantumRLAgent::updateNetworksSIMD() {
    // Update both networks using SIMD operations
    #pragma omp parallel sections
    {
        #pragma omp section
        optimizePolicyQuantum();
        
        #pragma omp section
        optimizeValueQuantum();
    }
}

} // namespace quids::rl