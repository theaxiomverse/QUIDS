#include "blockchain/Address.hpp"
#include <cstring>
#include <stdexcept>

namespace quids::blockchain {

    // Consider making these static constexpr class members

    Address::Address(const std::string& address) : address_(address) {
        // Optional: Add validation for address length
        if (address.length() != ADDRESS_SIZE) {
            throw std::invalid_argument("Invalid address length");
        }
    }

    Address::Address(const std::string& address, const std::array<uint8_t, 32>& location_hash)
            : address_(address), location_hash_(location_hash) {
        // Optional: Add validation for address length
        if (address.length() != ADDRESS_SIZE) {
            throw std::invalid_argument("Invalid address length");
        }
    }

    const std::string& Address::getAddress() const noexcept {
        return address_;
    }

    std::vector<uint8_t> Address::serialize() const {
        // Pre-allocate vector with exact size
        std::vector<uint8_t> data(TOTAL_SIZE);

        // Copy address directly using memcpy
        memcpy(data.data(), address_.data(), ADDRESS_SIZE);

        // Copy location hash
        memcpy(data.data() + ADDRESS_SIZE, location_hash_.data(), ADDRESS_HASH_SIZE);

        return data;
    }

    std::optional<Address> Address::deserialize(const std::vector<uint8_t>& data) {
        // Validate input size
        if (data.size() != TOTAL_SIZE) {
            return std::nullopt;
        }

        try {
            // Create address string directly from data
            std::string address(reinterpret_cast<const char*>(data.data()), ADDRESS_SIZE);

            // Create and fill location hash
            std::array<uint8_t, ADDRESS_HASH_SIZE> location_hash{};
            memcpy(location_hash.data(), data.data() + ADDRESS_SIZE, ADDRESS_HASH_SIZE);

            return Address(address, location_hash);
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }

} // namespace quids::blockchain
