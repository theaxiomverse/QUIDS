#include "crypto/QuantumHashFunction.hpp"
#include "quantum/QuantumCrypto.hpp"
#include <memory>
#include <vector>
#include <cstdint>
#include <chrono>

namespace quids {
    namespace crypto {

        class QuantumHashFunction::Impl {
        public:
            // Constructor with noexcept guarantee
            Impl() noexcept : quantum_crypto_() {}

            // Const member function since it doesn't modify the object state
            // Reference return type to avoid copy
            [[nodiscard]] const std::vector<uint8_t>& hash(const std::vector<uint8_t>& data) const noexcept {
                return quantum_crypto_.hashData(data);
            }

            // Overload for rvalue references (move semantics)
            const std::vector<uint8_t>& hash(std::vector<uint8_t>&& data) const noexcept {
                return quantum_crypto_.hashData(std::move(data));
            }


            void hashInto(const std::vector<uint8_t>& data, const std::vector<uint8_t>& output) const noexcept {
                return quantum_crypto_.hashInto(std::move(data), output);
            }





        private:
            quantum::QuantumCrypto quantum_crypto_;
        };

// Constructor with noexcept guarantee
        QuantumHashFunction::QuantumHashFunction() : impl_(std::make_unique<Impl>()) {}

        std::vector<uint8_t> QuantumHashFunction::hash(const std::vector<uint8_t>& data) const noexcept {
            return impl_->hash(data);
        }

// Virtual destructor since this is likely a base class
       QuantumHashFunction::~QuantumHashFunction() = default;

// Const member function with both lvalue and rvalue reference overloads


// Pre-allocated buffer version
        void QuantumHashFunction::hashInto(const std::vector<uint8_t>& data, const std::vector<uint8_t>& output) const noexcept {
            impl_->hashInto(data, output);
        }

    } // namespace crypto
} // namespace quids
