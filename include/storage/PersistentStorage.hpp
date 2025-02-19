#pragma once

#include "blockchain/Transaction.hpp"
#include "rollup/StateTransitionProof.hpp"
#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace quids {
namespace storage {

class PersistentStorage {
public:
    PersistentStorage(const std::string& data_dir);
    ~PersistentStorage();

    // Transaction storage
    [[maybe_unused]] [[maybe_unused]] bool storeTransaction(const blockchain::Transaction& tx);

    [[maybe_unused]] std::optional<blockchain::Transaction> loadTransaction(const std::array<uint8_t, 32>& tx_hash);
    std::vector<blockchain::Transaction> loadTransactions(uint64_t block_number);
    [[maybe_unused]] std::optional<std::unique_ptr<blockchain::Transaction>> loadTransaction(const std::array<uint8_t, 32>& tx_hash) const noexcept;


        // Proof storage
    bool storeProof(uint64_t block_number, const rollup::StateTransitionProof& proof);
    std::optional<rollup::StateTransitionProof> loadProof(uint64_t block_number);

    // Block storage
    bool storeBlockData(uint64_t block_number, const std::vector<uint8_t>& data);
    std::optional<std::vector<uint8_t>> loadBlockData(uint64_t block_number);

private:
    // Forward declaration of implementation
    struct Impl;  // Changed from class to struct
    std::unique_ptr<Impl> impl_;
};

} // namespace storage
} // namespace quids 