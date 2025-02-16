#pragma once

#include <memory>
#include <vector>
#include <array>
#include "../quantum/QuantumCircuit.hpp"
#include "../quantum/QuantumState.hpp"
#include "../neural/QuantumPolicyNetwork.hpp"
#include "../neural/QuantumValueNetwork.hpp"

namespace quids::rl {

struct RLMetrics {
    double averageReward;
    double episodeLength;
    double explorationRate;
    double learningRate;
    double discountFactor;
};

// Forward declarations
struct State {
    std::vector<double> values;
};

struct Action {
    size_t index;
};

struct Experience {
    State state;
    Action action;
    double reward;
    State nextState;
    bool done;
};

struct QuantumRLConfig {
    size_t stateSize;
    size_t actionSize;
    size_t numQubits;
    double learningRate;
    double discountFactor;
    size_t batchSize;
    size_t replayBufferSize;
    double explorationRate;
    ::quids::quantum::QuantumCircuitConfig circuitConfig;
};

class QuantumRLAgent {
public:
    explicit QuantumRLAgent(const QuantumRLConfig& config);
    ~QuantumRLAgent() = default;

    // Disable copy to prevent quantum state duplication
    QuantumRLAgent(const QuantumRLAgent&) = delete;
    QuantumRLAgent& operator=(const QuantumRLAgent&) = delete;

    // Allow move semantics
    QuantumRLAgent(QuantumRLAgent&&) noexcept = default;
    QuantumRLAgent& operator=(QuantumRLAgent&&) noexcept = default;

    // Core RL functions
    Action decideActionQuantum(const ::quids::quantum::QuantumState& state);
    void train(const std::vector<Experience>& experiences);
    void updateQuantumPolicy();

    // Configuration and metrics
    QuantumRLConfig getConfig() const;
    const RLMetrics& getMetrics() const;
    void setExplorationRate(double rate);

    // Metrics structure
    struct alignas(64) Metrics {
        double averageReward{0.0};
        double policyLoss{0.0};
        double valueLoss{0.0};
        double quantumFidelity{0.0};
        size_t trainingSteps{0};
    };

private:
    // Neural networks
    std::unique_ptr<::quids::neural::QuantumPolicyNetwork> policyNet_;
    std::unique_ptr<::quids::neural::QuantumValueNetwork> valueNet_;

    // Quantum circuit for state preparation and measurement
    std::unique_ptr<::quids::quantum::QuantumCircuit> circuit_;

    // Experience replay buffer (cache-aligned for SIMD)
    alignas(64) std::vector<Experience> replayBuffer_;

    // Configuration
    QuantumRLConfig config_;

    // Metrics
    Metrics metrics_;

    // Internal helper functions
    ::quids::quantum::QuantumState prepareQuantumState(const State& classicalState);
    Action measureQuantumState(const ::quids::quantum::QuantumState& state);
    double calculateQuantumAdvantage(const ::quids::quantum::QuantumState& state, const Action& action);
    void updateReplayBuffer(const Experience& experience);
    void optimizePolicyQuantum();
    void optimizeValueQuantum();

    // SIMD-optimized helper functions
    void processExperiencesBatch(const std::vector<Experience>& batch);
    void updateNetworksSIMD();

    // Constants
    static constexpr size_t CACHE_LINE_SIZE = 64;
    static constexpr size_t SIMD_WIDTH = 8;
    static constexpr size_t MIN_EXPERIENCES_FOR_TRAINING = 1000;
};

} // namespace quids::rl 