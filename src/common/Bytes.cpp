//
// Created by nick on 2025-02-19.
//

#include "common/Bytes.hpp"
#include <stdexcept>

namespace quids::common {

// Initialize static member
    constexpr char Bytes::HEX_CHARS[];

    Bytes::Bytes(const std::vector<uint8_t>& data) : __data(data) {
        // Using initializer list instead of assignment for better performance
    }

    std::string Bytes::toHex(size_t start, size_t end, size_t step, size_t offset) const {
        // Parameter validation
        if (start > end || end > __data.size()) {
            throw std::out_of_range("Invalid range parameters");
        }
        if (step == 0) {
            throw std::invalid_argument("Step size cannot be zero");
        }

        // Calculate result size and reserve memory
        const size_t elements = (end - start + step - 1) / step;
        std::string result;
        result.reserve(elements * 2 + offset);

        if (offset > 0) {
            result.append(offset, '0');
        }

        // Convert bytes to hex using optimized method
        result.resize(result.size() + ((end - start + step - 1) / step) * 2);
        char* dest = &result[offset];

        for (size_t i = start; i < end; i += step) {
            const uint8_t byte = __data[i];
            *dest++ = HEX_CHARS[(byte >> 4) & 0x0F];
            *dest++ = HEX_CHARS[byte & 0x0F];
        }

        return result;
    }

    void Bytes::append(const std::vector<uint8_t>& other) {
        if (other.empty()) {
            return;
        }

        const size_t new_size = __data.size() + other.size();
        __data.reserve(new_size);  // Reserve space to prevent multiple reallocations
        __data.insert(__data.end(), other.begin(), other.end());
    }

    void Bytes::append(const Bytes& other) {
        if (other.empty()) {
            return;
        }

        const size_t new_size = __data.size() + other.__data.size();
        __data.reserve(new_size);  // Reserve space to prevent multiple reallocations
        __data.insert(__data.end(), other.__data.begin(), other.__data.end());
    }

    std::vector<uint8_t> Bytes::subBytes(size_t start, size_t length) const {
        if (start > __data.size()) {
            throw std::out_of_range("Start index out of range");
        }

        const size_t available_length = __data.size() - start;
        const size_t actual_length = std::min(length, available_length);

        // Optimize for empty result case
        if (actual_length == 0) {
            return std::vector<uint8_t>();
        }

        // Pre-allocate the vector with exact size needed
        std::vector<uint8_t> result;
        result.reserve(actual_length);

        // Use direct iterator range construction
        return std::vector<uint8_t>(__data.begin() + start,
                                    __data.begin() + start + actual_length);
    }

    void Bytes::setBytes(const std::vector<uint8_t>& data) {
        __data = data;
    }

} // namespace quids::common
