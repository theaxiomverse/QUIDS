#pragma once

#include "quantum/QuantumState.hpp"
#include <memory>
#include <atomic>

namespace quids::consensus {

class QuantumConsensusModule {
public:
    explicit QuantumConsensusModule(bool useQuantumOptimization) 
        : useQuantum_(useQuantumOptimization) {}
    
    double getConsensusScore() const {
        return consensusScore_.load(std::memory_order_relaxed);
    }

private:
    std::atomic<double> consensusScore_{0.0};
    bool useQuantum_;
}; 