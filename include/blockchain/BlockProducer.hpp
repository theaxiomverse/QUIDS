#pragma once

#include "blockchain/AIBlock.hpp"
#include "blockchain/Transaction.hpp"
#include "quantum/QuantumState.hpp"
#include "neural/QuantumPolicyNetwork.hpp"
#include <memory>
#include <vector>
#include <chrono>

namespace quids::blockchain {

struct BlockProducerConfig {
    size_t maxTransactionsPerBlock;
    std::chrono::milliseconds blockTime;
    size_t minDifficulty;
    size_t maxDifficulty;
    double targetBlockTime;
    size_t numQubits;
};

class BlockProducer {
public:
    explicit BlockProducer(const BlockProducerConfig& config);
    ~BlockProducer() = default;

    // Disable copy
    BlockProducer(const BlockProducer&) = delete;
    BlockProducer& operator=(const BlockProducer&) = delete;

    // Allow move
    BlockProducer(BlockProducer&&) noexcept = default;
    BlockProducer& operator=(BlockProducer&&) noexcept = default;

    // Block production
    [[nodiscard]] AIBlock&& produceBlock(const std::vector<Transaction>& transactions);
    bool verifyBlock(const AIBlock& block) const;
    
    // Quantum-enhanced methods
    void updateQuantumState(const quantum::QuantumState& networkState);
    double getQuantumDifficulty() const;
    
    // Configuration
    void setConfig(const BlockProducerConfig& config);
    BlockProducerConfig getConfig() const;
    
    // Metrics
    struct Metrics {
        size_t blocksProduced{0};
        double averageBlockTime{0.0};
        double currentDifficulty{0.0};
        double quantumEntanglement{0.0};
        std::chrono::system_clock::time_point lastBlockTime;
    };
    
    const Metrics& getMetrics() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    // Helper methods
    void adjustDifficulty();
    bool validateTransactions(const std::vector<Transaction>& transactions) const;
    void updateMetrics(const AIBlock& block);
    quantum::QuantumState prepareQuantumState() const;
};

} // namespace quids::blockchain 