#pragma once

#include <vector>
#include <cstddef>
#include <chrono>
#include <stdexcept>
#include <string>
#include "consensus/OptimizedPOBPC.hpp"
#include "blockchain/Block.hpp"

namespace quids {
namespace core {

struct BlockProducerConfig {
    size_t max_transactions{1000};
    std::chrono::milliseconds batch_interval{1200}; // 1.2s for fast finality
    size_t witness_count{7};
    double consensus_threshold{0.67};
    bool use_quantum_proofs{true};
};

class BlockProducer {
public:
    explicit BlockProducer(const BlockProducerConfig& config = BlockProducerConfig{});
    ~BlockProducer() = default;

    // Block creation using POBPC
    std::vector<uint8_t> createBlock(const std::vector<uint8_t>& transactions);

    // Witness management
    bool registerWitness(const std::string& node_id, const std::vector<uint8_t>& public_key);
    bool submitWitnessVote(const std::string& witness_id, 
                          const std::vector<uint8_t>& signature,
                          const consensus::OptimizedPOBPC::BatchProof& proof);

    // Metrics
    consensus::OptimizedPOBPC::ConsensusMetrics getMetrics() const;

private:
    BlockProducerConfig config_;
    consensus::OptimizedPOBPC pobpc_;
};

} // namespace core
} // namespace quids 