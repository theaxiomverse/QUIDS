#include "core/BlockProducer.hpp"
#include <chrono>

namespace quids {
namespace core {

BlockProducer::BlockProducer(const BlockProducerConfig& config)
    : config_(config)
    , pobpc_(consensus::BatchConfig{
        .max_transactions = config.max_transactions,
        .witness_count = config.witness_count,
        .consensus_threshold = config.consensus_threshold,
        .use_quantum_proofs = config.use_quantum_proofs,
        .batch_size = config.max_transactions,
        .num_parallel_verifiers = 4,
        .quantum_circuit_depth = 20,
        .enable_error_correction = true
    }) {}

std::vector<uint8_t> BlockProducer::createBlock(const std::vector<uint8_t>& transactions) {
    // Add transactions to POBPC
    for (const auto& tx : transactions) {
        pobpc_.addTransaction(tx);
    }
    
    // Generate batch proof with fast finality
    auto proof = pobpc_.generateBatchProof();
    
    // Create block with the proof
    blockchain::Block block;
    block.transactions = transactions;
    block.proof = proof;
    block.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    
    // Verify consensus before returning
    if (!pobpc_.hasReachedConsensus(proof)) {
        throw std::runtime_error("Failed to reach consensus");
    }
    
    return block.serialize();
}

bool BlockProducer::registerWitness(const std::string& node_id, const std::vector<uint8_t>& public_key) {
    return pobpc_.registerWitness(node_id, public_key);
}

bool BlockProducer::submitWitnessVote(const std::string& witness_id,
                                     const std::vector<uint8_t>& signature,
                                     const consensus::OptimizedPOBPC::BatchProof& proof) {
    return pobpc_.submitWitnessVote(witness_id, signature, proof);
}

consensus::OptimizedPOBPC::ConsensusMetrics BlockProducer::getMetrics() const {
    return pobpc_.getMetrics();
}

} // namespace core
} // namespace quids 