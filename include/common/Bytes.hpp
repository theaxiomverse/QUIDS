//
// Created by nick on 2025-02-19.
//

#ifndef QUIDS_BYTES_HPP
#define QUIDS_BYTES_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>

namespace quids::common {

    class Bytes {
    private:
        std::vector<uint8_t> __data;

        // Helper method for hex conversion
        static constexpr char HEX_CHARS[] = "0123456789abcdef";

    public:
        // Constructors
        Bytes() = default;
        virtual ~Bytes() = default;
        explicit Bytes(const std::vector<uint8_t>& data);

        // Copy constructor and assignment operator
        Bytes(const Bytes&) = default;
        Bytes& operator=(const Bytes&) = default;

        // Move constructor and assignment operator
        Bytes(Bytes&&) noexcept = default;
        Bytes& operator=(Bytes&&) noexcept = default;

        // Data access methods
        [[nodiscard]] const std::vector<uint8_t>& getBytes() const noexcept { return __data; }
        [[nodiscard]] size_t size() const noexcept { return __data.size(); }
        [[nodiscard]] bool empty() const noexcept { return __data.empty(); }

        // Utility methods
        void append(const std::vector<uint8_t>& other);
        void append(const Bytes& other);
        [[nodiscard]] std::vector<uint8_t> subBytes(size_t start, size_t length) const;
        void setBytes(const std::vector<uint8_t>& data);

        // Hex conversion methods
        [[nodiscard]] std::string toHex() const {
            return toHex(0, __data.size());
        }

        [[nodiscard]] std::string toHex(size_t start) const {
            return toHex(start, __data.size());
        }

        [[nodiscard]] std::string toHex(size_t start, size_t end) const {
            return toHex(start, end, 1, 0);
        }

        [[nodiscard]] std::string toHex(size_t start, size_t end, size_t step) const {
            return toHex(start, end, step, 0);
        }

        [[nodiscard]] std::string toHex(size_t start, size_t end, size_t step, size_t offset) const;

        // Iterator support
        using iterator = std::vector<uint8_t>::iterator;
        using const_iterator = std::vector<uint8_t>::const_iterator;

        [[nodiscard]] iterator begin() noexcept { return __data.begin(); }
        [[nodiscard]] iterator end() noexcept { return __data.end(); }
        [[nodiscard]] const_iterator begin() const noexcept { return __data.begin(); }
        [[nodiscard]] const_iterator end() const noexcept { return __data.end(); }
        [[nodiscard]] const_iterator cbegin() const noexcept { return __data.cbegin(); }
        [[nodiscard]] const_iterator cend() const noexcept { return __data.cend(); }

        // Element access
        uint8_t& operator[](size_t index) { return __data[index]; }
        const uint8_t& operator[](size_t index) const { return __data[index]; }
        uint8_t& at(size_t index) { return __data.at(index); }
        const uint8_t& at(size_t index) const { return __data.at(index); }

        // Modification methods
        void clear() noexcept { __data.clear(); }
        void resize(size_t new_size) { __data.resize(new_size); }
        void reserve(size_t new_capacity) { __data.reserve(new_capacity); }
        void push_back(uint8_t value) { __data.push_back(value); }

        // Comparison operators
        bool operator==(const Bytes& other) const noexcept {
            return __data == other.__data;
        }
        bool operator!=(const Bytes& other) const noexcept {
            return !(*this == other);
        }
    };

} // namespace quids::common

#endif // QUIDS_BYTES_HPP
