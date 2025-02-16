#include "crypto/blake3/Blake3Hash.hpp"
#include <blake3.h>
#include <stdexcept>
#include <memory>

namespace quids {
namespace crypto {

class Blake3Hash::Impl {
public:
    Impl() {
        blake3_hasher_init(&hasher_);
    }
    
    void update(const uint8_t* data, size_t len) {
        blake3_hasher_update(&hasher_, data, len);
    }
    
    std::vector<uint8_t> finalize() {
        std::vector<uint8_t> hash(BLAKE3_OUT_LEN);
        blake3_hasher_finalize(&hasher_, hash.data(), BLAKE3_OUT_LEN);
        return hash;
    }
    
private:
    blake3_hasher hasher_;
};

Blake3Hash::Blake3Hash() : impl_(std::make_unique<Impl>()) {}
Blake3Hash::~Blake3Hash() = default;

void Blake3Hash::update(const std::vector<uint8_t>& data) {
    impl_->update(data.data(), data.size());
}

void Blake3Hash::update(const uint8_t* data, size_t len) {
    impl_->update(data, len);
}

std::vector<uint8_t> Blake3Hash::finalize() {
    return impl_->finalize();
}

} // namespace crypto
} // namespace quids