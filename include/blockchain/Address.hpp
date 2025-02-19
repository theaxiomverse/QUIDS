#ifndef QUIDS_BLOCKCHAIN_ADDRESS_HPP
#define QUIDS_BLOCKCHAIN_ADDRESS_HPP

#include <string>
#include <array>
#include <vector>
#include <optional>

namespace quids { namespace blockchain {

        static constexpr size_t ADDRESS_SIZE = 42;
   static constexpr size_t ADDRESS_HASH_SIZE = 32;
        static constexpr size_t TOTAL_SIZE = ADDRESS_SIZE + ADDRESS_HASH_SIZE;

        class Address {
        public:
            Address() = default;
            explicit Address(const std::string& address);
            Address(const std::string& address, const std::array<uint8_t, 32>& location_hash);

            [[nodiscard]] const std::string& getAddress() const noexcept;
            [[nodiscard]] std::vector<uint8_t> serialize() const;
            static std::optional<Address> deserialize(const std::vector<uint8_t>& data);

        private:
            std::string address_;
            std::array<uint8_t, 32> location_hash_{};
        };

    } } // namespace quids::blockchain

#endif // QUIDS_BLOCKCHAIN_ADDRESS_HPP