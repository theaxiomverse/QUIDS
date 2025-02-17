#ifndef QUIDS_BLOCKCHAIN_STANDARD_TRANSACTION_HPP
#define QUIDS_BLOCKCHAIN_STANDARD_TRANSACTION_HPP

#include "blockchain/Transaction.hpp"
#include "crypto/blake3/Blake3Hash.hpp"
#include <string>
#include <vector>

namespace quids::blockchain {

class StandardTransaction : public Transaction {
public:
    StandardTransaction() = default;
    ~StandardTransaction() override = default;

    // Core virtual methods
    void serialize(ByteVector& out) const override;
    bool deserialize(const ByteVector& data) override;
    [[nodiscard]] std::vector<uint8_t> computeHash() const override;
    [[nodiscard]] bool verify() const override;
    [[nodiscard]] ::std::string toString() const override;

    // Interface methods
    [[nodiscard]] Address getFrom() const override { return sender; }
    [[nodiscard]] Address getTo() const override { return receiver; }
    [[nodiscard]] GasPrice getGasPrice() const override { return gas_cost; }
    [[nodiscard]] GasLimit getGasLimit() const override { return 21000; }
    [[nodiscard]] uint64_t getNonce() const noexcept override { return nonce; }

    // Setter methods
    void setSender(const Address& s) noexcept { sender = s; }
    void setReceiver(const Address& r) noexcept { receiver = r; }
    void setValue(Value v) noexcept { value = v; }
    void setData(const ByteVector& d) { data = d; }
    void setSignature(const Signature& s) noexcept { signature = s; }
    void setTimestamp(Timestamp t) noexcept { timestamp = t; }
    void setNonce(uint64_t n) noexcept { nonce = n; }
    void setGasCost(uint64_t g) noexcept { gas_cost = g; }

    // Implement pure virtual methods from Transaction
    [[nodiscard]] const Address& getSender() const noexcept  { return sender; }
    [[nodiscard]] const Address& getReceiver() const noexcept { return receiver; }
    [[nodiscard]] Value getValue() const noexcept { return value; }
    [[nodiscard]] const ByteVector& getData() const noexcept { return data; }
    [[nodiscard]] const Signature& getSignature() const noexcept { return signature; }
    [[nodiscard]] Timestamp getTimestamp() const noexcept { return timestamp; }

private:
    Address sender;
    Address receiver;
    Value value{0};
    ByteVector data;
    Signature signature;
    Timestamp timestamp{::std::chrono::system_clock::now()};
    uint64_t nonce{0};
    uint64_t gas_cost{0};
};

} // namespace quids::blockchain

#endif // QUIDS_BLOCKCHAIN_STANDARD_TRANSACTION_HPP 