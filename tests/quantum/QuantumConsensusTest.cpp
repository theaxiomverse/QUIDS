#include "quantum/QuantumConsensus.hpp"
#include "quantum/QuantumUtils.hpp"
#include "consensus/OptimizedPOBPC.hpp"
#include <gtest/gtest.h>
#include <random>
#include <cmath>
#include <chrono>

namespace quids::quantum::consensus::test {

using quantum::QuantumState;
using ::std::random_device;
using ::std::mt19937;
using ::std::uniform_real_distribution;
using ::std::max_element;
using ::std::distance;
using ::std::chrono::milliseconds;
using quids::consensus::BatchConfig;
using quids::consensus::OptimizedPOBPC;
using ::std::string;
using ::std::to_string;
using ::std::vector;
using ::std::size_t;

/**
 * @brief Test fixture for quantum consensus protocols
 * 
 * Provides common setup and utilities for testing quantum consensus mechanisms:
 * - Byzantine agreement
 * - Leader election
 * - Distributed key generation
 */
class QuantumConsensusTest : public ::testing::Test {
protected:
    void SetUp() override {
        config.num_parties = 4;
        config.num_qubits_per_party = 2;
        config.error_threshold = 0.01;
        config.max_rounds = 10;
        config.use_entanglement = true;
        config.timeout_seconds = 1.0;
    }

    /**
     * @brief Creates initial quantum states for testing
     * @param num_states Number of states to create
     * @return Vector of initialized quantum states
     */
    vector<QuantumState> createInitialStates(size_t num_states) {
        vector<QuantumState> states;
        for (size_t i = 0; i < num_states; ++i) {
            QuantumState state(::std::size_t(config.num_qubits_per_party));
            state.applyHadamard(0);  // Create superposition
            states.push_back(state);
        }
        return states;
    }

    ConsensusConfig config;
};

/**
 * @brief Tests Byzantine agreement protocol
 */
TEST_F(QuantumConsensusTest, ByzantineAgreement) {
    auto initial_states = createInitialStates(config.num_parties);
    
    auto result = quantumByzantineAgreement(config, initial_states);
    
    EXPECT_TRUE(result.success);
    EXPECT_LE(result.rounds_taken, config.max_rounds);
    EXPECT_EQ(result.measurements.size(), config.num_parties);
    EXPECT_TRUE(verifyConsensus(result, config));
}

/**
 * @brief Tests Byzantine agreement with faulty parties
 */
TEST_F(QuantumConsensusTest, ByzantineAgreementWithFaults) {
    config.num_parties = 7;  // Allow for 2 faulty parties (f < n/3)
    auto initial_states = createInitialStates(config.num_parties);
    
    // Simulate faulty parties by applying random operations
    ::std::random_device rd;
    ::std::mt19937 gen(rd());
    ::std::uniform_real_distribution<> dis(0, 2 * M_PI);
    
    for (size_t i = 0; i < 2; ++i) {  // Corrupt 2 parties
        initial_states[i].applyPhase(0, dis(gen));
    }
    
    auto result = quantumByzantineAgreement(config, initial_states);
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(verifyConsensus(result, config));
}

/**
 * @brief Tests quantum leader election protocol
 */
TEST_F(QuantumConsensusTest, LeaderElection) {
    auto result = quantumLeaderElection(config);
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.rounds_taken, 1);
    EXPECT_EQ(result.measurements.size(), config.num_parties);
    
    // Verify unique leader was elected
    size_t leaders = 0;
    for (auto m : result.measurements) {
        if (m == 1) leaders++;
    }
    EXPECT_EQ(leaders, 1);
}

/**
 * @brief Tests quantum leader election fairness
 */
TEST_F(QuantumConsensusTest, LeaderElectionFairness) {
    const size_t NUM_TRIALS = 1000;
    vector<size_t> leader_counts(config.num_parties, 0);
    
    for (size_t i = 0; i < NUM_TRIALS; ++i) {
        auto result = quantumLeaderElection(config);
        auto max_it = ::std::max_element(result.measurements.begin(), result.measurements.end());
        size_t leader_idx = ::std::distance(result.measurements.begin(), max_it);
        leader_counts[leader_idx]++;
    }
    
    // Check fairness (each party should be leader ~25% of the time)
    double expected = NUM_TRIALS / config.num_parties;
    double tolerance = expected * 0.2;  // Allow 20% deviation
    
    for (auto count : leader_counts) {
        EXPECT_NEAR(count, expected, tolerance);
    }
}

/**
 * @brief Tests quantum distributed key generation
 */
TEST_F(QuantumConsensusTest, DistributedKeyGeneration) {
    auto result = quantumDistributedKeyGeneration(config);
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.measurements.size(), config.num_parties);
    
    // Verify key properties
    for (auto m : result.measurements) {
        EXPECT_TRUE(m == 0 || m == 1);  // Binary outcomes
    }
}

/**
 * @brief Tests quantum distributed key generation security
 */
TEST_F(QuantumConsensusTest, DistributedKeyGenerationSecurity) {
    const size_t NUM_TRIALS = 1000;
    vector<size_t> bit_counts(2, 0);  // Count 0s and 1s
    
    for (size_t i = 0; i < NUM_TRIALS; ++i) {
        auto result = quantumDistributedKeyGeneration(config);
        for (auto m : result.measurements) {
            bit_counts[m]++;
        }
    }
    
    // Check randomness (should have roughly equal 0s and 1s)
    double total_bits = bit_counts[0] + bit_counts[1];
    double ratio_0 = bit_counts[0] / total_bits;
    double ratio_1 = bit_counts[1] / total_bits;
    
    EXPECT_NEAR(ratio_0, 0.5, 0.1);
    EXPECT_NEAR(ratio_1, 0.5, 0.1);
}

/**
 * @brief Tests consensus verification
 */
TEST_F(QuantumConsensusTest, ConsensusVerification) {
    ConsensusResult valid_result;
    valid_result.success = true;
    valid_result.rounds_taken = 5;
    valid_result.measurements = {1, 1, 1, 0};  // 75% agreement
    
    ConsensusResult invalid_result;
    invalid_result.success = true;
    invalid_result.rounds_taken = config.max_rounds + 1;
    
    EXPECT_TRUE(verifyConsensus(valid_result, config));
    EXPECT_FALSE(verifyConsensus(invalid_result, config));
}

/**
 * @brief Test fixture for POBPC consensus
 */
class POBPCTest : public ::testing::Test {
protected:
    void SetUp() override {
        config.max_transactions = 1000;
        config.witness_count = 4;
        config.consensus_threshold = 0.75;
        config.use_quantum_proofs = true;
        config.batch_size = 256;
        config.num_parallel_verifiers = 8;
        config.quantum_circuit_depth = 32;
        config.enable_error_correction = true;
        config.batch_timeout = milliseconds(1000);
        config.witness_selection_entropy = 1.0;
        config.min_witness_reliability = 80;
        config.max_batch_verification_time = 500;
        config.adaptive_witness_selection = true;
        config.recursive_zkp_layers = 2;
    }

    BatchConfig config;
};

/**
 * @brief Tests batch proof generation and verification
 */
TEST_F(POBPCTest, BatchProofGeneration) {
    OptimizedPOBPC consensus(config);
    
    // Create test transactions
    for (size_t i = 0; i < 100; ++i) {
        vector<uint8_t> tx{1, 2, 3, 4};  // Sample transaction data
        bool success = consensus.addTransaction(tx);
        EXPECT_TRUE(success);
    }
    
    // Generate batch proof
    auto proof = consensus.generateBatchProof();
    
    EXPECT_TRUE(proof.isValid());
    EXPECT_EQ(proof.transaction_count, 100);
    EXPECT_FALSE(proof.batch_hash.empty());
    EXPECT_FALSE(proof.proof_data.empty());
    EXPECT_TRUE(proof.zkp_data.isComplete());
}

/**
 * @brief Tests witness selection and reliability tracking
 */
TEST_F(POBPCTest, WitnessSelection) {
    OptimizedPOBPC consensus(config);
    
    // Register test witnesses
    for (size_t i = 0; i < 10; ++i) {
        string witness_id = "witness_" + to_string(i);
        vector<uint8_t> pubkey{1, 2, 3, 4};  // Sample public key
        bool success = consensus.registerWitness(witness_id, pubkey);
        EXPECT_TRUE(success);
    }
    
    // Test witness selection
    auto selected_witnesses = consensus.selectWitnesses();
    
    EXPECT_EQ(selected_witnesses.size(), config.witness_count);
    
    // Verify witness properties
    for (const auto& witness : selected_witnesses) {
        EXPECT_FALSE(witness.node_id.empty());
        EXPECT_FALSE(witness.public_key.empty());
        EXPECT_GE(witness.reliability_score.load(), 0.0);
        EXPECT_LE(witness.reliability_score.load(), 1.0);
    }
}

/**
 * @brief Tests batch consensus with quantum proofs
 */
TEST_F(POBPCTest, QuantumEnhancedConsensus) {
    OptimizedPOBPC consensus(config);
    
    // Create and add transactions
    for (size_t i = 0; i < config.batch_size; ++i) {
        vector<uint8_t> tx{1, 2, 3, 4};  // Sample transaction data
        bool success = consensus.addTransaction(tx);
        EXPECT_TRUE(success);
    }
    
    // Generate proof with quantum enhancement
    auto proof = consensus.generateBatchProof();
    
    // Verify quantum security properties
    EXPECT_TRUE(proof.quantum_state.isValid());
    EXPECT_FALSE(proof.measurements.empty());
    EXPECT_GT(proof.metrics.quantum_enhancement_factor, 1.0);
    
    // Verify the proof
    EXPECT_TRUE(consensus.verifyBatchProof(proof));
}

/**
 * @brief Tests consensus metrics and performance
 */
TEST_F(POBPCTest, ConsensusMetrics) {
    OptimizedPOBPC consensus(config);
    
    // Generate some test activity
    for (size_t i = 0; i < 5; ++i) {
        vector<vector<uint8_t>> batch;
        for (size_t j = 0; j < config.batch_size; ++j) {
            batch.push_back(vector<uint8_t>{1, 2, 3, 4});
        }
        
        for (const auto& tx : batch) {
            bool success = consensus.addTransaction(tx);
            EXPECT_TRUE(success);
        }
        
        auto proof = consensus.generateBatchProof();
        EXPECT_TRUE(consensus.verifyBatchProof(proof));
    }
    
    // Check metrics
    auto metrics = consensus.getMetrics();
    
    EXPECT_GT(metrics.avg_batch_time.load(), 0.0);
    EXPECT_GT(metrics.avg_verification_time.load(), 0.0);
    EXPECT_GT(metrics.total_batches.load(), 0);
    EXPECT_GT(metrics.total_transactions.load(), 0);
    EXPECT_GE(metrics.witness_participation.load(), 0.0);
    EXPECT_LE(metrics.witness_participation.load(), 1.0);
    EXPECT_GT(metrics.quantum_security.load(), 0.0);
    EXPECT_GE(metrics.quantum_fidelity.load(), 0.0);
    EXPECT_LE(metrics.quantum_fidelity.load(), 1.0);
    
    // Check batch-specific metrics
    EXPECT_GT(metrics.batch_metrics.avg_batch_size.load(), 0.0);
    EXPECT_GT(metrics.batch_metrics.batch_formation_time.load(), 0.0);
    EXPECT_GE(metrics.batch_metrics.batch_success_rate.load(), 0.0);
    EXPECT_LE(metrics.batch_metrics.batch_success_rate.load(), 1.0);
    
    // Check witness metrics
    EXPECT_GT(metrics.witness_metrics.avg_witness_count.load(), 0.0);
    EXPECT_GE(metrics.witness_metrics.witness_reliability.load(), 0.0);
    EXPECT_LE(metrics.witness_metrics.witness_reliability.load(), 1.0);
    
    // Check ZKP metrics
    EXPECT_GT(metrics.zkp_metrics.avg_proof_size.load(), 0.0);
    EXPECT_GE(metrics.zkp_metrics.verification_success.load(), 0.0);
    EXPECT_LE(metrics.zkp_metrics.verification_success.load(), 1.0);
    EXPECT_GE(metrics.zkp_metrics.quantum_speedup.load(), 1.0);
}

/**
 * @brief Tests fault tolerance and recovery
 */
TEST_F(POBPCTest, FaultTolerance) {
    OptimizedPOBPC consensus(config);
    
    // Register witnesses including some potentially faulty ones
    for (size_t i = 0; i < 10; ++i) {
        string witness_id = "witness_" + to_string(i);
        vector<uint8_t> pubkey{1, 2, 3, 4};
        consensus.registerWitness(witness_id, pubkey);
    }
    
    // Create a batch with some transactions
    for (size_t i = 0; i < config.batch_size; ++i) {
        vector<uint8_t> tx{1, 2, 3, 4};  // Sample transaction data
        bool success = consensus.addTransaction(tx);
        EXPECT_TRUE(success);
    }
    
    auto proof = consensus.generateBatchProof();
    
    // Simulate some witness failures
    size_t successful_votes = 0;
    for (size_t i = 0; i < proof.witness_data.selected_witnesses.size(); ++i) {
        if (i < proof.witness_data.selected_witnesses.size() * 2/3) {
            // Simulate successful votes for 2/3 of witnesses
            vector<uint8_t> signature{1, 2, 3, 4};  // Valid signature
            bool success = consensus.submitWitnessVote(
                proof.witness_data.selected_witnesses[i],
                signature,
                proof
            );
            EXPECT_TRUE(success);
            successful_votes++;
        }
        // Remaining witnesses don't vote (simulating failures)
    }
    
    // Verify consensus can be reached with partial witness participation
    EXPECT_TRUE(consensus.hasReachedConsensus(proof));
    EXPECT_GE(successful_votes, config.witness_count * 2/3);
    
    // Check consensus confidence
    double confidence = consensus.calculateConsensusConfidence(proof);
    EXPECT_GE(confidence, config.consensus_threshold);
}

} // namespace quids::quantum::consensus::test 