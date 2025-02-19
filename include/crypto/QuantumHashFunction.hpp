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
    [[nodiscard]] std::vector<uint8_t> hash(const std::vector<uint8_t>& data) const noexcept;
   void hashInto(const std::vector<uint8_t>& data, const std::vector<uint8_t>& output) const noexcept;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace crypto
} // namespace quids