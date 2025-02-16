#pragma once

#include <atomic>
#include <vector>

namespace quids::state {

class LockFreeStateManager {
public:
    explicit LockFreeStateManager(bool useParallelProcessing) 
        : useParallel_(useParallelProcessing) {}
    
    void updateState(double value) {
        currentValue_.store(value, std::memory_order_relaxed);
    }

private:
    std::atomic<double> currentValue_{0.0};
    bool useParallel_;
}; 
}