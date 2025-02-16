#pragma once

#include <vector>
#include <cstdint>
#include <array>
#include <string>
#include <chrono>

namespace quids::blockchain {

class Transaction;

class Block {
public:
    Block() = default;
    Block(const Block&) = default;
    Block(Block&&) noexcept = default;
    Block& operator=(const Block&) = default;
    Block& operator=(Block&&) noexcept = default;

    // Core block data
    uint64_t number{0};
    std::array<uint8_t, 32> previousHash;
    std::array<uint8_t, 32> merkleRoot;
    std::array<uint8_t, 32> stateRoot;
    std::vector<Transaction> transactions;
    uint64_t timestamp{0};
    uint64_t nonce{0};
    uint32_t difficulty{0};
    std::vector<uint8_t> signature;
    std::array<uint8_t, 32> hash;

    // Virtual methods for serialization
    virtual std::vector<uint8_t> serialize() const;
    virtual void deserialize(const std::vector<uint8_t>& data);

    // Virtual methods for block operations
    virtual bool addTransaction(const Transaction& tx);
    virtual bool verifyBlock() const;
    virtual std::array<uint8_t, 32> computeHash() const;
    virtual std::array<uint8_t, 32> computeMerkleRoot() const;

protected:
    // Protected constructor to prevent direct instantiation
    Block() = default;
    Block(const Block&) = default;
    Block& operator=(const Block&) = default;
    Block(Block&&) noexcept = default;
    Block& operator=(Block&&) noexcept = default;
};

} // namespace quids::blockchain 