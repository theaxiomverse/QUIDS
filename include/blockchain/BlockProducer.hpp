#ifndef QUIDS_BLOCKCHAIN_BLOCK_PRODUCER_HPP
#define QUIDS_BLOCKCHAIN_BLOCK_PRODUCER_HPP

#include "AIBlock.hpp"
#include "blockchain/Transaction.hpp"
#include "quantum/QuantumState.hpp"
#include "neural/QuantumPolicyNetwork.hpp"
#include <memory>
#include <vector>
#include <chrono>

namespace quids::blockchain {

struct BlockProducerConfig {
    std::size_t maxTransactionsPerBlock;
    std::chrono::milliseconds blockTime;
    std::size_t minDifficulty;
    std::size_t maxDifficulty;
    double targetBlockTime;
    std::size_t numQubits;
};

class BlockProducer {
public:
    explicit BlockProducer(const BlockProducerConfig& config);
    ~BlockProducer() = default;

    // Disable copy operations
    BlockProducer(const BlockProducer&) = delete;
    BlockProducer& operator=(const BlockProducer&) = delete;

    // Enable move operations
    BlockProducer(BlockProducer&&) noexcept = default;
    BlockProducer& operator=(BlockProducer&&) noexcept = default;

    // Block production
    [[nodiscard]] AIBlock produceBlock(const std::vector<Transaction>& transactions);
    [[nodiscard]] bool verifyBlock(const AIBlock& block) const noexcept;
    
    // Quantum-enhanced methods
    void updateQuantumState(const quantum::QuantumState& networkState);
    [[nodiscard]] double getQuantumDifficulty() const noexcept;
    
    // Configuration
    void setConfig(const BlockProducerConfig& config);
    [[nodiscard]] BlockProducerConfig getConfig() const noexcept;
    
    // Metrics
    struct Metrics {
        std::size_t blocksProduced{0};
        double averageBlockTime{0.0};
        double currentDifficulty{0.0};
        double quantumEntanglement{0.0};
        std::chrono::system_clock::time_point lastBlockTime;
    };
    
    [[nodiscard]] const Metrics& getMetrics() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    // Helper methods
    void adjustDifficulty();
    [[nodiscard]] bool validateTransactions(const std::vector<Transaction>& transactions) const;
    void updateMetrics(const AIBlock& block);
    [[nodiscard]] quantum::QuantumState prepareQuantumState() const;
};

} // namespace quids::blockchain

#endif // QUIDS_BLOCKCHAIN_BLOCK_PRODUCER_HPP 