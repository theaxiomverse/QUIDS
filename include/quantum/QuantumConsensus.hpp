#pragma once

#include "QuantumForward.hpp"
#include <memory>
#include <vector>
#include <optional>

namespace quids::quantum {

struct ConsensusMessage;

class QuantumConsensus {
public:
    static std::unique_ptr<QuantumConsensus> create();
    virtual ~QuantumConsensus() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    
    virtual void process_message(const ConsensusMessage& msg) = 0;
    virtual std::optional<ConsensusMessage> get_next_message() = 0;
    
    virtual double get_consensus_score() const = 0;
    virtual bool is_consensus_reached() const = 0;

protected:
    QuantumConsensus() = default;
};

struct ConsensusMessage {
    enum class Type {
        PROPOSAL,
        VOTE,
        COMMIT,
        FINALIZE
    };
    
    Type type;
    std::vector<uint8_t> data;
    QuantumState quantum_state;
    double confidence_score;
};

} // namespace quids::quantum 