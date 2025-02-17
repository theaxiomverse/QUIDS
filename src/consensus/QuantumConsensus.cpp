#include "consensus/QuantumConsensus.hpp"
#include "crypto/blake3/Blake3Hash.hpp"
#include <fstream>
#include <cmath>
#include <algorithm>

namespace quids::consensus {

void QuantumConsensusModule::processTransaction(const blockchain::Transaction& tx) {
    if (!useQuantum_) return;
    
    // Extract transaction features for quantum processing
    ::std::vector<double> features;
    features.reserve(4);
    features.push_back(static_cast<double>(tx.getNonce()));
    features.push_back(static_cast<double>(tx.getValue()));
    
    // Update quantum state based on transaction features
    for (const auto& feature : features) {
        quantumState_->applyRotation(feature);
    }
    
    // Apply quantum circuit for transaction processing
    applyQuantumCircuit();
    
    // Update consensus metrics
    updateConsensusScore();
    processedTxCount_.fetch_add(1, ::std::memory_order_relaxed);
}

void QuantumConsensusModule::optimize() {
    if (!useQuantum_) return;
    
    // Optimize quantum circuit parameters
    optimizeQuantumCircuit();
    
    // Update quantum state
    updateQuantumState();
    
    // Recalculate consensus score
    updateConsensusScore();
}

void QuantumConsensusModule::applyQuantumCircuit() {
    if (!useQuantum_) return;
    
    const size_t numQubits = quantumState_->getNumQubits();
    
    // Apply Hadamard gates
    for (size_t i = 0; i < numQubits; ++i) {
        quantumState_->applyHadamard(i);
    }
    
    // Apply CNOT gates between adjacent qubits
    for (size_t i = 0; i < numQubits - 1; ++i) {
        quantumState_->applyCNOT(i, i + 1);
    }
    
    // Apply parameterized rotation gates
    for (size_t i = 0; i < numQubits; ++i) {
        if (i < quantumParameters_.size()) {
            quantumState_->applyRotation(i, quantumParameters_[i]);
        }
    }
}

void QuantumConsensusModule::updateQuantumState() {
    if (!useQuantum_) return;
    
    // Normalize quantum state
    quantumState_->normalize();
    
    // Update entanglement metrics
    double entanglementScore = calculateEntanglementScore();
    consensusMetrics_.push_back(entanglementScore);
}

double QuantumConsensusModule::calculateQuantumAdvantage() const {
    if (!useQuantum_) return 1.0;
    
    // Calculate quantum advantage based on state purity and entanglement
    double statePurity = quantumState_->getStateVector().norm();
    double entanglementScore = calculateEntanglementScore();
    
    return ::std::sqrt(statePurity * entanglementScore);
}

double QuantumConsensusModule::calculateEntanglementScore() const {
    if (!useQuantum_) return 0.0;
    
    // Calculate von Neumann entropy as entanglement measure
    const auto& stateVector = quantumState_->getStateVector();
    double entropy = 0.0;
    
    for (const auto& amplitude : stateVector) {
        double prob = ::std::norm(amplitude);
        if (prob > 1e-10) {  // Avoid log(0)
            entropy -= prob * ::std::log2(prob);
        }
    }
    
    return entropy;
}

bool QuantumConsensusModule::verifyQuantumState() const {
    if (!useQuantum_) return true;
    
    // Verify state vector normalization
    const auto& stateVector = quantumState_->getStateVector();
    double norm = stateVector.norm();
    
    if (::std::abs(norm - 1.0) > 1e-6) {
        return false;
    }
    
    // Verify quantum advantage is within expected range
    double advantage = calculateQuantumAdvantage();
    return advantage >= 0.0 && advantage <= 1.0;
}

void QuantumConsensusModule::reset() {
    processedTxCount_.store(0, ::std::memory_order_relaxed);
    consensusScore_.store(0.0, ::std::memory_order_relaxed);
    quantumParameters_.clear();
    consensusMetrics_.clear();
    
    if (useQuantum_) {
        quantumState_->reset();
    }
}

void QuantumConsensusModule::saveState(const ::std::string& filePath) const {
    ::std::ofstream file(filePath, ::std::ios::binary);
    if (!file) {
        throw ::std::runtime_error("Failed to open file for saving quantum state");
    }
    
    // Save quantum parameters
    size_t paramSize = quantumParameters_.size();
    file.write(reinterpret_cast<const char*>(&paramSize), sizeof(paramSize));
    file.write(reinterpret_cast<const char*>(quantumParameters_.data()), 
               paramSize * sizeof(double));
    
    // Save consensus metrics
    size_t metricSize = consensusMetrics_.size();
    file.write(reinterpret_cast<const char*>(&metricSize), sizeof(metricSize));
    file.write(reinterpret_cast<const char*>(consensusMetrics_.data()),
               metricSize * sizeof(double));
    
    // Save quantum state
    if (useQuantum_) {
        quantumState_->serialize(file);
    }
}

void QuantumConsensusModule::loadState(const ::std::string& filePath) {
    ::std::ifstream file(filePath, ::std::ios::binary);
    if (!file) {
        throw ::std::runtime_error("Failed to open file for loading quantum state");
    }
    
    // Load quantum parameters
    size_t paramSize;
    file.read(reinterpret_cast<char*>(&paramSize), sizeof(paramSize));
    quantumParameters_.resize(paramSize);
    file.read(reinterpret_cast<char*>(quantumParameters_.data()),
              paramSize * sizeof(double));
    
    // Load consensus metrics
    size_t metricSize;
    file.read(reinterpret_cast<char*>(&metricSize), sizeof(metricSize));
    consensusMetrics_.resize(metricSize);
    file.read(reinterpret_cast<char*>(consensusMetrics_.data()),
              metricSize * sizeof(double));
    
    // Load quantum state
    if (useQuantum_) {
        quantumState_->deserialize(file);
    }
}

void QuantumConsensusModule::updateConsensusScore() {
    double quantumAdvantage = calculateQuantumAdvantage();
    double entanglementScore = calculateEntanglementScore();
    double transactionEntropy = computeTransactionEntropy();
    
    // Combine metrics with appropriate weights
    double newScore = 0.4 * quantumAdvantage +
                     0.3 * entanglementScore +
                     0.3 * transactionEntropy;
    
    consensusScore_.store(newScore, ::std::memory_order_relaxed);
}

void QuantumConsensusModule::optimizeQuantumCircuit() {
    if (!useQuantum_) return;
    
    // Gradient descent optimization of quantum parameters
    const double learningRate = 0.01;
    const size_t numParams = quantumParameters_.size();
    
    ::std::vector<double> gradients(numParams);
    
    // Compute gradients
    for (size_t i = 0; i < numParams; ++i) {
        // Finite difference approximation
        const double epsilon = 1e-6;
        quantumParameters_[i] += epsilon;
        double costPlus = calculateQuantumAdvantage();
        
        quantumParameters_[i] -= 2 * epsilon;
        double costMinus = calculateQuantumAdvantage();
        
        quantumParameters_[i] += epsilon;
        gradients[i] = (costPlus - costMinus) / (2 * epsilon);
    }
    
    // Update parameters
    for (size_t i = 0; i < numParams; ++i) {
        quantumParameters_[i] += learningRate * gradients[i];
    }
}

double QuantumConsensusModule::computeTransactionEntropy() const {
    if (processedTxCount_.load(::std::memory_order_relaxed) == 0) {
        return 0.0;
    }
    
    // Use consensus metrics history to compute entropy
    double entropy = 0.0;
    double sum = 0.0;
    
    for (const auto& metric : consensusMetrics_) {
        sum += metric;
    }
    
    if (sum > 0.0) {
        for (const auto& metric : consensusMetrics_) {
            double p = metric / sum;
            if (p > 0.0) {
                entropy -= p * ::std::log2(p);
            }
        }
    }
    
    return entropy;
}

} // namespace quids::consensus 