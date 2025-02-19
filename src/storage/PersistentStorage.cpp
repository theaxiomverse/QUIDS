#include "storage/PersistentStorage.hpp"
#include "blockchain/StandardTransaction.hpp"
#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>
#include <sstream>
#include <filesystem>
#include <utility>
#include <iostream> // For logging

namespace quids {
    namespace storage {

        class Transaction;

        /**
         * @brief Implementation details for PersistentStorage.
         */
        struct PersistentStorage::Impl {
            std::unique_ptr<rocksdb::DB> db; ///< Unique pointer to the RocksDB database instance.
            std::string data_dir; ///< Directory path for storing database files.

            /**
             * @brief Constructor to initialize the database.
             * @param dir Directory path for the database.
             */
            explicit Impl(std::string dir) : data_dir(std::move(dir)) {
                rocksdb::Options options;
                options.create_if_missing = true;
                options.compression = rocksdb::kLZ4Compression;
                options.max_background_jobs = 4;
                options.write_buffer_size = 64 * 1024 * 1024; // 64MB
                options.target_file_size_base = 64 * 1024 * 1024; // 64MB

                rocksdb::DB* db_ptr = nullptr;
                rocksdb::Status status = rocksdb::DB::Open(options, data_dir, &db_ptr);
                if (!status.ok()) {
                    std::cerr << "Failed to open database: " << status.ToString() << std::endl;
                    throw std::runtime_error("Failed to open database: " + status.ToString());
                }
                db.reset(db_ptr);
            }

            /**
             * @brief Creates a key for the database.
             * @param prefix Prefix for the key.
             * @param id Identifier for the key.
             * @return Combined key as a string.
             */
            std::string makeKey(const std::string& prefix, const std::string& id) {
                return prefix + ":" + id;
            }
        };

        /**
         * @brief Constructor for PersistentStorage.
         * @param data_dir Directory path for the database.
         */
        PersistentStorage::PersistentStorage(const std::string& data_dir)
                : impl_(std::make_unique<Impl>(data_dir)) {}

        /**
         * @brief Destructor for PersistentStorage.
         */
        PersistentStorage::~PersistentStorage() = default;

        /**
         * @brief Stores a transaction in the database.
         * @param tx Transaction to be stored.
         * @return True if the transaction was stored successfully, false otherwise.
         */
        bool PersistentStorage::storeTransaction(const blockchain::Transaction& tx) {
            std::vector<uint8_t> out;
            tx.serialize(out);
            auto hash = tx.computeHash();
            std::string key = impl_->makeKey("tx", std::string(hash.begin(), hash.end()));

            rocksdb::Status status = impl_->db->Put(
                    rocksdb::WriteOptions(),
                    key,
                    rocksdb::Slice(reinterpret_cast<const char*>(out.data()), out.size())
            );

            if (!status.ok()) {
                std::cerr << "Failed to store transaction: " << status.ToString() << std::endl;
            }

            return status.ok();
        }

        /**
         * @brief Loads a transaction from the database.
         * @param tx_hash Hash of the transaction to be loaded.
         * @return Optional containing the loaded transaction if successful, nullopt otherwise.
         */
        std::optional<std::unique_ptr<blockchain::Transaction>> PersistentStorage::loadTransaction(const std::array<uint8_t, 32>& tx_hash) const noexcept {
        std::string key = impl_->makeKey("tx", std::string(tx_hash.begin(), tx_hash.end()));
        std::string tx_data;

        rocksdb::Status status = impl_->db->Get(rocksdb::ReadOptions(), key, &tx_data);

        if (status.ok()) {
        std::vector<uint8_t> data(tx_data.begin(), tx_data.end());
        auto tx = std::make_unique<blockchain::StandardTransaction>();
        if (tx->deserialize(data)) {
        return tx;
    } else {
    std::cerr << "Failed to deserialize transaction data." << std::endl;
}
} else {
std::cerr << "Failed to load transaction: " << status.ToString() << std::endl;
}

return std::nullopt;
}
/**
 * @brief Stores a state transition proof in the database.
 * @param block_number Block number associated with the proof.
 * @param proof State transition proof to be stored.
 * @return True if the proof was stored successfully, false otherwise.
 */
bool PersistentStorage::storeProof(uint64_t block_number, const rollup::StateTransitionProof& proof) {
    auto serialized = proof.serialize();
    std::string key = impl_->makeKey("proof", std::to_string(block_number));

    rocksdb::Status status = impl_->db->Put(
            rocksdb::WriteOptions(),
            key,
            rocksdb::Slice(reinterpret_cast<const char*>(serialized.data()), serialized.size())
    );

    if (!status.ok()) {
        std::cerr << "Failed to store proof: " << status.ToString() << std::endl;
    }

    return status.ok();
}

/**
 * @brief Loads a state transition proof from the database.
 * @param block_number Block number associated with the proof.
 * @return Optional containing the loaded proof if successful, nullopt otherwise.
 */
std::optional<rollup::StateTransitionProof> PersistentStorage::loadProof(uint64_t block_number) {
    std::string key = impl_->makeKey("proof", std::to_string(block_number));
    std::string proof_data;

    rocksdb::Status status = impl_->db->Get(
            rocksdb::ReadOptions(),
            key,
            &proof_data
    );

    if (status.ok()) {
        std::vector<uint8_t> data(proof_data.begin(), proof_data.end());
        return rollup::StateTransitionProof::deserialize(data);
    } else {
        std::cerr << "Failed to load proof: " << status.ToString() << std::endl;
    }

    return std::nullopt;
}

/**
 * @brief Stores block data in the database.
 * @param block_number Block number associated with the data.
 * @param data Block data to be stored.
 * @return True if the data was stored successfully, false otherwise.
 */
bool PersistentStorage::storeBlockData(uint64_t block_number, const std::vector<uint8_t>& data) {
    std::string key = impl_->makeKey("block", std::to_string(block_number));

    rocksdb::Status status = impl_->db->Put(
            rocksdb::WriteOptions(),
            key,
            rocksdb::Slice(reinterpret_cast<const char*>(data.data()), data.size())
    );

    if (!status.ok()) {
        std::cerr << "Failed to store block data: " << status.ToString() << std::endl;
    }

    return status.ok();
}

/**
 * @brief Loads block data from the database.
 * @param block_number Block number associated with the data.
 * @return Optional containing the loaded block data if successful, nullopt otherwise.
 */
std::optional<std::vector<uint8_t>> PersistentStorage::loadBlockData(uint64_t block_number) {
    std::string key = impl_->makeKey("block", std::to_string(block_number));
    std::string block_data;

    rocksdb::Status status = impl_->db->Get(
            rocksdb::ReadOptions(),
            key,
            &block_data
    );

    if (status.ok()) {
        return std::vector<uint8_t>(block_data.begin(), block_data.end());
    } else {
        std::cerr << "Failed to load block data: " << status.ToString() << std::endl;
    }

    return std::nullopt;
}

/**
 * @brief Loads transactions for a specific block number.
 * @param block_number Block number associated with the transactions.
 * @return Vector containing the loaded transactions.
 */
std::vector<blockchain::Transaction> PersistentStorage::loadTransactions(uint64_t /*block_number*/) {
    // Implementation for loading transactions
    return std::vector<blockchain::Transaction>();
}

} // namespace storage
} // namespace quids