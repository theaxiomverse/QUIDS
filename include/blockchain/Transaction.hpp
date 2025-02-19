#ifndef QUIDS_BLOCKCHAIN_TRANSACTION_HPP
#define QUIDS_BLOCKCHAIN_TRANSACTION_HPP

#include "Types.hpp"
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <chrono>
#include "Address.hpp"

namespace quids::blockchain {

// Forward declaration only
class [[maybe_unused]] StandardTransaction;

class Transaction {
public:
    Transaction() = default;
    virtual ~Transaction() = default;

    // Core virtual methods
    virtual void serialize(ByteVector& out) const = 0;
    virtual bool deserialize(const ByteVector& data) = 0;
    [[nodiscard]] virtual std::vector<uint8_t> computeHash() const = 0;
    [[nodiscard]] virtual bool verify() const = 0;
    virtual ::std::string toString() const = 0;

    // Getter methods
    [[nodiscard]] const Address& getSender() const noexcept { return sender; }
    [[nodiscard]] const Address& getRecipient() const noexcept { return receiver; }
    [[nodiscard]] const Value& getValue() const noexcept { return value; }
    [[nodiscard]] const Data& getData() const noexcept { return data; }
    [[nodiscard]] const Signature& getSignature() const noexcept { return signature; }
    [[nodiscard]] const Timestamp& getTimestamp() const noexcept { return timestamp; }
    [[nodiscard]] uint64_t getAmount() const noexcept { return value; }
    [[nodiscard]] uint64_t calculate_gas_cost() const noexcept { return gas_cost; }

    // Virtual interface methods
    [[nodiscard]] virtual Address getFrom() const = 0;
    [[nodiscard]] virtual Address getTo() const = 0;
    [[nodiscard]] virtual GasPrice getGasPrice() const = 0;
    [[nodiscard]] virtual GasLimit getGasLimit() const = 0;
    [[nodiscard]] virtual uint64_t getNonce() const noexcept = 0;

    [[nodiscard]] virtual std::string bytesToHex(const std::vector<uint8_t>& bytes);

protected:
    Timestamp timestamp{std::chrono::system_clock::now()};
    Address sender;
    Address receiver;
    Value value{0};
    Data data;
    Signature signature{};
    uint64_t nonce{0};
    uint64_t gas_cost{0};
};

} // namespace quids::blockchain

#endif // QUIDS_BLOCKCHAIN_TRANSACTION_HPP 