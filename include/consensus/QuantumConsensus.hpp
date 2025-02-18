#pragma once

#include "quantum/QuantumState.hpp"
#include "blockchain/Transaction.hpp"
#include "quantum/QuantumCircuit.hpp"
#include <memory>
#include <atomic>
#include <vector>

namespace quids::consensus {

class QuantumConsensusModule {
private:
    bool useQuantum_;
    ::std::atomic<double> consensusScore_;
    ::std::atomic<::std::size_t> processedTxCount_{0};
    ::std::unique_ptr<quantum::QuantumState> quantumState_;
    ::std::unique_ptr<quantum::QuantumCircuit> quantumCircuit_;
    ::std::vector<double> quantumParameters_;
    ::std::vector<double> consensusMetrics_;

public:
    explicit QuantumConsensusModule(bool useQuantumOptimization) 
        : useQuantum_(useQuantumOptimization),
          consensusScore_(0.0),
          quantumState_(::std::make_unique<quantum::QuantumState>(8)),
          quantumCircuit_(::std::make_unique<quantum::QuantumCircuit>(8)) {}
    
    // Core consensus methods
    void processTransaction(const blockchain::Transaction& tx);
    void optimize();
    [[nodiscard]] double getConsensusScore() const {
        return consensusScore_.load(::std::memory_order_relaxed);
    }
    
    // Quantum-specific methods
    void applyQuantumCircuit();
    void updateQuantumState();
    [[nodiscard]] double calculateQuantumAdvantage() const;
    [[nodiscard]] double calculateEntanglementScore() const;
    [[nodiscard]] bool verifyQuantumState() const;
    
    // State management
    void reset();
    void saveState(const ::std::string& filePath) const;
    void loadState(const ::std::string& filePath);
    
    // Getters
    [[nodiscard]] bool isQuantumEnabled() const noexcept { return useQuantum_; }
    [[nodiscard]] const quantum::QuantumState& getQuantumState() const { return *quantumState_; }
    [[nodiscard]] ::std::size_t getProcessedTransactionCount() const { return processedTxCount_; }

private:
    // Private helper methods
    void updateConsensusScore();
    void optimizeQuantumCircuit();
    [[nodiscard]] double computeTransactionEntropy() const;
}; 

} // namespace quids::consensus


