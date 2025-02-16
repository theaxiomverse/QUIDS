#include "crypto/QuantumHashFunction.hpp"
#include "quantum/QuantumCrypto.hpp"
#include <memory>

namespace quids {
namespace crypto {

class QuantumHashFunction::Impl {
public:
    Impl() : quantum_crypto_() {}
    
    std::vector<uint8_t> hash(const std::vector<uint8_t>& data) {
        return quantum_crypto_.hashData(data);
    }
    
private:
    quantum::QuantumCrypto quantum_crypto_;
};

QuantumHashFunction::QuantumHashFunction() : impl_(std::make_unique<Impl>()) {}
QuantumHashFunction::~QuantumHashFunction() = default;

std::vector<uint8_t> QuantumHashFunction::hash(const std::vector<uint8_t>& data) {
    return impl_->hash(data);
}

} // namespace crypto
} // namespace quids