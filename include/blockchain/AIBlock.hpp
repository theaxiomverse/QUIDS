#ifndef QUIDS_BLOCKCHAIN_AI_BLOCK_HPP
#define QUIDS_BLOCKCHAIN_AI_BLOCK_HPP

#include "Block.hpp"
#include "Transaction.hpp"
#include "Types.hpp"
#include <atomic>
#include <mutex>
#include <memory>
#include <chrono>
#include <array>
#include <optional>
#include <vector>
#include <functional>

namespace quids::blockchain {

struct AIMetrics {
    ::std::atomic<double> throughput{0.0};
    ::std::atomic<double> latency{0.0};
    ::std::atomic<double> quantumAdvantage{0.0};
    ::std::chrono::system_clock::time_point lastUpdateTime{::std::chrono::system_clock::now()};
};

struct AIBlockConfig {
    ::std::size_t numQubits{8};
    ::std::size_t modelInputSize{16};
    ::std::size_t modelOutputSize{8};
    ::std::size_t maxTransactionsPerBlock{1024};
    ::std::size_t batchSize{128};
    ::std::size_t quantumCircuitDepth{4};
    double learningRate{0.001};
    bool useQuantumOptimization{true};
    bool useParallelProcessing{true};
    bool useSIMD{true};
};

class AIBlock : public Block {
public:
    struct Impl;

    explicit AIBlock(const AIBlockConfig& config);
    ~AIBlock() override;

    // Delete copy operations
    AIBlock(const AIBlock&) = delete;
    AIBlock& operator=(const AIBlock&) = delete;

    // Delete move operations (due to mutex)
    AIBlock(AIBlock&&) = delete;
    AIBlock& operator=(AIBlock&&) = delete;

    // Block interface implementation
    bool addTransaction(const Transaction& tx) override;
    bool verify() const override;
    ByteArray hash() const override;
    void computeHash() override;
    void computeMerkleRoot() override;
    void applyQuantumOptimization() override;
    double calculateQuantumSecurityScore() const override;

    // AIBlock specific methods
    const ::quids::blockchain::AIMetrics& getMetrics() const;
    AIBlockConfig getConfig() const;
    void serialize(ByteVector& out) const override;
    void deserialize(const ByteVector& in) override;

protected:
    void processBlockParallel();
    void processTransactionsSIMD(const ::std::vector<::std::reference_wrapper<const Transaction>>& batch);
    void applyQuantumCircuit(QuantumState& state) const;
    ::std::vector<double> extractFeatures(const Transaction& tx) const;
    double computeTransactionEntropy() const;
    void updateMetrics();
    void optimizeParameters();
    void invalidateCache();

private:
    ::std::unique_ptr<Impl> impl_;
    ::std::unique_ptr<memory::MemoryPool<Transaction>> txPool_;
    ::std::unique_ptr<state::LockFreeStateManager> stateManager_;
    ::std::unique_ptr<consensus::QuantumConsensusModule> consensusModule_;
    ::std::unique_ptr<rl::QuantumRLAgent> rlAgent_;
    ::std::unique_ptr<neural::QuantumPolicyNetwork> policyNetwork_;
    ::std::unique_ptr<neural::QuantumValueNetwork> valueNetwork_;
    ::std::unique_ptr<quantum::QuantumState> quantumState_;
    ::std::unique_ptr<quantum::QuantumCircuit> quantumCircuit_;
    mutable ::std::mutex mutex_;
    ::std::optional<ByteArray> cachedHash_;
    ::std::optional<ByteArray> cachedMerkleRoot_;
    uint64_t blockNumber_{0};
    ByteArray previousHash_{};
    ByteArray merkleRoot_{};
    ::std::chrono::system_clock::time_point timestamp_{::std::chrono::system_clock::now()};
    uint64_t nonce_{0};
    uint64_t difficulty_{0};
    ::std::vector<double> stateBuffer_;
};

} // namespace quids::blockchain

#endif // QUIDS_BLOCKCHAIN_AI_BLOCK_HPP

