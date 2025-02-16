#pragma once

#include <vector>
#include <memory>
#include <mutex>

namespace quids::memory {

template<typename T>
class MemoryPool {
public:
    explicit MemoryPool(size_t initialSize) : size_(initialSize) {}
    
    T* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        return new T();
    }
    
    void deallocate(T* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        delete ptr;
    }

private:
    size_t size_;
    std::mutex mutex_;
}; 