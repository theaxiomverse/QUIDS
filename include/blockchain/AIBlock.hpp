#pragma once

#include "blockchain/Block.hpp"
#include "blockchain/Transaction.hpp"
#include "quantum/QuantumState.hpp"
#include "neural/QuantumPolicyNetwork.hpp"
#include "memory/MemoryPool.hpp"
#include "state/LockFreeStateManager.hpp"
#include "consensus/QuantumConsensus.hpp"
#include "rl/QuantumRLAgent.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <mutex>
#include <optional>
#include <atomic>
#include <array>

namespace quids::blockchain {

struct AIBlockConfig {
    size_t modelInputSize{256};
    size_t modelOutputSize{64};
    size_t numQubits{8};
    double learningRate{0.001};
    size_t batchSize{32};
    bool useQuantumOptimization{true};
    size_t maxTransactionsPerBlock{1000};
    double targetBlockTime{15.0}; // seconds
    size_t quantumCircuitDepth{4};
    bool useErrorCorrection{true};
    bool useParallelProcessing{true};
    bool useSIMD{true};
};

class AIBlock : public Block {
public:
    explicit AIBlock(const AIBlockConfig& config);
    ~AIBlock() override = default;

    // Disable copy
    AIBlock(const AIBlock&) = delete;
    AIBlock& operator=(const AIBlock&) = delete;

    // Allow move
    AIBlock(AIBlock&&) noexcept = default;
    AIBlock& operator=(AIBlock&&) noexcept = default;

    // Override Block virtual methods
    bool addTransaction(const Transaction& tx) override;
    bool verifyBlock() const override;
    std::array<uint8_t, 32> computeHash() const override;
    std::array<uint8_t, 32> computeMerkleRoot() const override;
    std::vector<uint8_t> serialize() const override;
    void deserialize(const std::vector<uint8_t>& data) override;

    // AI-specific methods with optimizations
    void updateModel(const std::vector<Transaction>& transactions);
    std::vector<double> predictNextState() const;
    double getModelConfidence() const;
    std::optional<double> predictOptimalGasPrice() const;
    std::vector<Transaction> suggestOptimalTransactionOrder() const;
    
    // Quantum integration
    void applyQuantumOptimization();
    quantum::QuantumState getQuantumState() const;
    void setQuantumState(const quantum::QuantumState& state);
    double calculateQuantumSecurityScore() const;
    
    // Optimized processing
    void processBlockParallel();
    void finalizeBlockZK();
    
    // Enhanced metrics and analysis
    struct alignas(64) AIMetrics {
        std::atomic<double> modelLoss{0.0};
        std::atomic<double> predictionAccuracy{0.0};
        std::atomic<double> quantumAdvantage{0.0};
        std::atomic<double> securityScore{0.0};
        std::atomic<double> transactionEfficiency{0.0};
        std::atomic<double> networkHealth{0.0};
        std::atomic<double> throughput{0.0};
        std::atomic<double> latency{0.0};
        std::atomic<double> energyUsage{0.0};
        std::atomic<int> validatorCount{0};
        std::atomic<size_t> trainingSteps{0};
        std::chrono::system_clock::time_point lastUpdateTime;
        quantum::QuantumSecurityMetrics quantumMetrics;
        std::vector<double> historicalPredictions;
        std::vector<double> historicalAccuracy;
    };
    
    const AIMetrics& getMetrics() const;
    AIBlockConfig getConfig() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

    // Optimized components
    memory::MemoryPool<Transaction> txPool_;
    state::LockFreeStateManager stateManager_;
    consensus::QuantumConsensusModule consensus_;
    rl::QuantumRLAgent agent_;
    
    // Cache-aligned state
    alignas(64) std::array<double, 8> stateBuffer_;

    // Helper methods
    void trainOnBatch(const std::vector<Transaction>& batch);
    void updateMetrics();
    void optimizeParameters();
    std::vector<double> extractFeatures(const Transaction& tx) const;
    void applyQuantumCircuit(quantum::QuantumState& state) const;
    double computeTransactionEntropy() const;
    void processTransactionsSIMD(const std::vector<Transaction>& batch);
    void applyOptimizationsParallel();
    
    // Thread safety
    mutable std::mutex mutex_;
    
    // Caching
    mutable std::optional<std::array<uint8_t, 32>> cachedHash_;
    mutable std::optional<std::array<uint8_t, 32>> cachedMerkleRoot_;
    void invalidateCache();

    // Constants
    static constexpr size_t CACHE_LINE_SIZE = 64;
    static constexpr size_t SIMD_WIDTH = 8;
    static constexpr size_t MAX_BATCH_SIZE = 1024;
};

} // namespace quids::blockchain 