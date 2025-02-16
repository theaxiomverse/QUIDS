#pragma once

#include <memory>
#include <vector>
#include <cstdint>

namespace quids {
namespace crypto {

class QuantumHashFunction {
public:
    QuantumHashFunction();
    ~QuantumHashFunction();
    
    // Hash data using quantum-resistant algorithm
    std::vector<uint8_t> hash(const std::vector<uint8_t>& data);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace crypto
} // namespace quids