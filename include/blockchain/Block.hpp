#ifndef QUIDS_BLOCKCHAIN_BLOCK_HPP
#define QUIDS_BLOCKCHAIN_BLOCK_HPP

#include "Types.hpp"
#include "Transaction.hpp"
#include <vector>
#include <memory>

/**
 * @file Block.hpp
 * @brief Base blockchain block implementation
 * 
 * This file defines the base Block class that serves as the foundation
 * for all block types in the blockchain. It provides core functionality
 * for transaction management, hashing, and block validation.
 */

namespace quids::blockchain {

/**
 * @brief Base class for blockchain blocks
 * 
 * Provides core block functionality including transaction management,
 * Merkle tree computation, and block validation. This class serves
 * as the base for specialized block types.
 */
class Block {
public:
    virtual ~Block() = default;

    // Disable copy
    Block(const Block&) = delete;
    Block& operator=(const Block&) = delete;

    // Allow move
    Block(Block&&) noexcept = default;
    Block& operator=(Block&&) noexcept = default;

    // Pure virtual methods that derived classes must implement
    virtual bool addTransaction(const Transaction& tx) = 0;
    virtual bool verify() const = 0;
    virtual Hash hash() const = 0;
    virtual void computeHash() = 0;
    virtual void computeMerkleRoot() = 0;
    virtual void applyQuantumOptimization() = 0;
    virtual double calculateQuantumSecurityScore() const = 0;
    virtual void serialize(ByteVector& out) const = 0;
    virtual void deserialize(const ByteVector& in) = 0;

protected:
    Block() = default;

    // Protected members
    Hash previousHash_;
    Hash merkleRoot_;
    BlockNumber number_{0};
    Timestamp timestamp_{::std::chrono::system_clock::now()};
    ::std::vector<Transaction> transactions_;
    ::std::vector<Hash> transactionHashes_;
    ::std::vector<double> quantumStates_;
    ::std::vector<double> quantumAmplitudes_;
    ::std::vector<double> quantumPhases_;
    ::std::vector<double> quantumEntanglement_;
    ::std::vector<double> quantumSuperposition_;
    ::std::vector<double> quantumDecoherence_;
    ::std::vector<double> quantumInterference_;
    ::std::vector<double> quantumTeleportation_;
    ::std::vector<double> quantumErrorCorrection_;
    ::std::vector<double> quantumGates_;
    ::std::vector<double> quantumMeasurements_;
    ::std::vector<double> quantumFeatures_;
    ::std::vector<double> quantumMetrics_;
    ::std::vector<double> quantumParameters_;
    ::std::vector<double> quantumGradients_;
    ::std::vector<double> quantumOptimization_;
    ::std::vector<double> quantumLearning_;
    ::std::vector<double> quantumPredictions_;
    ::std::vector<double> quantumProbabilities_;
    ::std::vector<double> quantumStatistics_;
    ::std::vector<double> quantumCorrelations_;
    ::std::vector<double> quantumCoherence_;
    ::std::vector<double> quantumEntropy_;
    ::std::vector<double> quantumFidelity_;
    ::std::vector<double> quantumPurity_;
    ::std::vector<double> quantumConcurrence_;
    ::std::vector<double> quantumDiscord_;
    ::std::vector<double> quantumNegentropy_;
    ::std::vector<double> quantumInformation_;
    ::std::vector<double> quantumComplexity_;
    ::std::vector<double> quantumCapacity_;
    ::std::vector<double> quantumEfficiency_;
    ::std::vector<double> quantumRobustness_;
    ::std::vector<double> quantumResilience_;
    ::std::vector<double> quantumSecurity_;
    ::std::vector<double> quantumPrivacy_;
    ::std::vector<double> quantumAuthenticity_;
    ::std::vector<double> quantumIntegrity_;
    ::std::vector<double> quantumConfidentiality_;
    ::std::vector<double> quantumAvailability_;
    ::std::vector<double> quantumReliability_;
    ::std::vector<double> quantumPerformance_;
    ::std::vector<double> quantumScalability_;
    ::std::vector<double> quantumThroughput_;
    ::std::vector<double> quantumLatency_;
    ::std::vector<double> quantumBandwidth_;
    ::std::vector<double> quantumUtilization_;
    ::std::vector<double> quantumEffectiveness_;
    ::std::vector<double> quantumQuality_;
};

} // namespace quids::blockchain

#endif // QUIDS_BLOCKCHAIN_BLOCK_HPP 