#ifndef QUIDS_BLOCKCHAIN_BLOCK_HPP
#define QUIDS_BLOCKCHAIN_BLOCK_HPP

#include "StdNamespace.hpp"
#include "Transaction.hpp"

/**
 * @file Block.hpp
 * @brief Base blockchain block implementation
 * 
 * This file defines the base Block class that serves as the foundation
 * for all block types in the blockchain. It provides core functionality
 * for transaction management, hashing, and block validation.
 */

namespace quids::blockchain {

/**
 * @brief Base class for blockchain blocks
 * 
 * Provides core block functionality including transaction management,
 * Merkle tree computation, and block validation. This class serves
 * as the base for specialized block types.
 */
class Block {
public:
    Block() = default;
    virtual ~Block() = default;

    // Disable copy
    Block(const Block&) = delete;
    Block& operator=(const Block&) = delete;

    // Allow move
    Block(Block&&) noexcept = default;
    Block& operator=(Block&&) noexcept = default;

    // Core block data
    uint64_t number{0};                          ///< Block number in chain
    std::array<uint8_t, 32> previousHash;      ///< Hash of previous block
    std::array<uint8_t, 32> merkleRoot;        ///< Merkle tree root hash
    std::array<uint8_t, 32> stateRoot;         ///< State trie root hash
    std::vector<Transaction> transactions;             ///< Block transactions
    uint64_t timestamp{0};                       ///< Block creation time
    uint64_t nonce{0};                          ///< Mining nonce
    uint32_t difficulty{0};                      ///< Mining difficulty
    std::vector<uint8_t> signature;            ///< Block signature
    std::array<uint8_t, 32> hash;             ///< Block hash

    /**
     * @brief Adds a transaction to the block
     * @param tx Transaction to add
     * @return true if transaction was added successfully
     */
    virtual bool addTransaction(const Transaction& tx) = 0;

    /**
     * @brief Verifies block integrity
     * @return true if block is valid
     */
    virtual bool verifyBlock() const = 0;

    /**
     * @brief Computes block hash
     * @return 32-byte block hash
     */
    virtual std::array<uint8_t, 32> computeHash() const = 0;

    /**
     * @brief Computes Merkle root of transactions
     * @return 32-byte Merkle root hash
     */
        virtual std::array<uint8_t, 32> computeMerkleRoot() const = 0;

    /**
     * @brief Serializes block data
     * @return Serialized block data
     */
    virtual std::vector<uint8_t> serialize() const = 0;

    /**
     * @brief Deserializes block data
     * @param data Serialized block data
     */virtual void deserialize(const std::vector<uint8_t>& data) = 0;
};

} // namespace quids::blockchain

#endif // QUIDS_BLOCKCHAIN_BLOCK_HPP 