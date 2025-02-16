#pragma once

#include <vector>
#include <cstdint>
#include <memory>

namespace quids {
namespace crypto {

class Blake3Hash {
public:
    Blake3Hash();
    ~Blake3Hash();

    // Delete copy and move to ensure proper RAII
    Blake3Hash(const Blake3Hash&) = delete;
    Blake3Hash& operator=(const Blake3Hash&) = delete;
    Blake3Hash(Blake3Hash&&) = delete;
    Blake3Hash& operator=(Blake3Hash&&) = delete;

    // Update hash state with data
    void update(const std::vector<uint8_t>& data);
    void update(const uint8_t* data, size_t len);

    // Finalize and get 256-bit hash
    std::vector<uint8_t> finalize();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace crypto
} // namespace quids