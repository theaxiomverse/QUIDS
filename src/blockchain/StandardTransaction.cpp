#include "blockchain/StandardTransaction.hpp"
#include "blockchain/Address.hpp"
#include "crypto/blake3/Blake3Hash.hpp"
#include <sstream>
#include <iomanip>
#include <cstring>

namespace quids::blockchain {

    void StandardTransaction::serialize(ByteVector& out) const {
        // Get serialized addresses
        auto serialized_sender = sender.serialize();
        auto serialized_receiver = receiver.serialize();

        // Calculate total size needed
        const size_t total_size = sizeof(Timestamp) +
                                  serialized_sender.size() +
                                  serialized_receiver.size() +
                                  sizeof(Value) +
                                  data.size() +
                                  sizeof(Signature) +
                                  2 * sizeof(uint64_t);

        // Resize once instead of multiple inserts
        out.resize(total_size);
        size_t offset = 0;

        // Copy timestamp
        memcpy(out.data() + offset, &timestamp, sizeof(Timestamp));
        offset += sizeof(Timestamp);

        // Copy sender address
        memcpy(out.data() + offset, serialized_sender.data(), serialized_sender.size());
        offset += serialized_sender.size();

        // Copy receiver address
        memcpy(out.data() + offset, serialized_receiver.data(), serialized_receiver.size());
        offset += serialized_receiver.size();

        // Copy value
        memcpy(out.data() + offset, &value, sizeof(Value));
        offset += sizeof(Value);

        // Copy transaction data
        memcpy(out.data() + offset, data.data(), data.size());
        offset += data.size();

        // Copy signature
        memcpy(out.data() + offset, signature.data(), signature.size());
        offset += signature.size();

        // Copy nonce and gas cost
        memcpy(out.data() + offset, &nonce, sizeof(uint64_t));
        offset += sizeof(uint64_t);
        memcpy(out.data() + offset, &gas_cost, sizeof(uint64_t));
    }

    bool StandardTransaction::deserialize(const ByteVector& data) {
        const size_t address_size = TOTAL_SIZE; // Assuming this is defined in Address class
        const size_t min_size = sizeof(Timestamp) + 2 * address_size + sizeof(Value) +
                                sizeof(Signature) + 2 * sizeof(uint64_t);

        if (data.size() < min_size) {
            return false;
        }

        const uint8_t* ptr = data.data();
        size_t offset = 0;

        // Deserialize timestamp
        memcpy(&timestamp, ptr + offset, sizeof(Timestamp));
        offset += sizeof(Timestamp);

        // Deserialize sender address
        std::vector<uint8_t> sender_data(ptr + offset, ptr + offset + address_size);
        auto maybe_sender = Address::deserialize(sender_data);
        if (!maybe_sender) return false;
        sender = *maybe_sender;
        offset += address_size;

        // Deserialize receiver address
        std::vector<uint8_t> receiver_data(ptr + offset, ptr + offset + address_size);
        auto maybe_receiver = Address::deserialize(receiver_data);
        if (!maybe_receiver) return false;
        receiver = *maybe_receiver;
        offset += address_size;

        // Deserialize value
        memcpy(&value, ptr + offset, sizeof(Value));
        offset += sizeof(Value);

        // Deserialize transaction data
        const size_t data_size = data.size() - offset - sizeof(Signature) - 2 * sizeof(uint64_t);
        this->data.resize(data_size);
        memcpy(this->data.data(), ptr + offset, data_size);
        offset += data_size;

        // Deserialize signature
        memcpy(signature.data(), ptr + offset, sizeof(Signature));
        offset += sizeof(Signature);

        // Deserialize nonce and gas cost
        memcpy(&nonce, ptr + offset, sizeof(uint64_t));
        offset += sizeof(uint64_t);
        memcpy(&gas_cost, ptr + offset, sizeof(uint64_t));

        return true;
    }

    std::vector<uint8_t> StandardTransaction::computeHash() const {
        // Pre-calculate the size needed for serialization
        const size_t serialized_size = sizeof(Timestamp) + 2 * sizeof(Address) +
                                       sizeof(Value) + data.size() +
                                       sizeof(Signature) + 2 * sizeof(uint64_t);
        ByteVector serialized;
        serialized.reserve(serialized_size);  // Reserve exact space needed
        serialize(serialized);

        Blake3Hash hasher;
        hasher.update(serialized.data(), serialized.size());
        return hasher.finalize();
    }

    std::string StandardTransaction::toString() const {



        static constexpr std::string_view LABELS[] = {
                "Timestamp: ",
                "\nSender: ",
                "\nReceiver: ",
                "\nValue: ",
                "\nData: ",
                "\nSignature: ",
                "\nNonce: ",
                "\nGas Cost: ",
                "\n"
        };

        std::string result;
        result.reserve(256);

        auto time_t_timestamp = std::chrono::system_clock::to_time_t(timestamp);
        std::tm tm = *std::gmtime(&time_t_timestamp);  // Use gmtime for UTC
        char time_str[32];
        std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S UTC", &tm);

        result.append(LABELS[0])
                .append(time_str)
                .append(LABELS[1])
                .append(sender.getAddress())
                .append(LABELS[2])
                .append(receiver.getAddress())
                .append(LABELS[3])
                .append(std::to_string(value))
                .append(LABELS[4]);


        if (data.empty()) {
            result.append("empty");
        } else {
            result.append(StandardTransaction::bytesToHex(data));
        }

        result.append(LABELS[5]);

        // Hex conversion for signature
        static const char hex_chars[] = "0123456789abcdef";
        for (const auto& byte : signature) {
            result.push_back(hex_chars[byte >> 4]);
            result.push_back(hex_chars[byte & 0x0F]);
        }

        result.append(LABELS[6])
                .append(std::to_string(nonce))
                .append(LABELS[7])
                .append(std::to_string(gas_cost))
                .append(LABELS[8]);

        return result;
    }


    std::string StandardTransaction::bytesToHex(const std::vector<uint8_t>& bytes) const {
        static const char hex_chars[] = "0123456789abcdef";
        std::string result;
        result.reserve(bytes.size() * 2);

        for (const auto& byte : bytes) {
            result.push_back(hex_chars[byte >> 4]);
            result.push_back(hex_chars[byte & 0x0F]);
        }
        return result;
    }


    bool StandardTransaction::verify() const {
        if (sender.getAddress().empty() || receiver.getAddress().empty()) {
            return false;
        }

        // Create serialized addresses
        auto serialized_sender = sender.serialize();
        auto serialized_receiver = receiver.serialize();

        // Calculate message size
        const size_t msg_size = sizeof(Timestamp) +
                                serialized_sender.size() +
                                serialized_receiver.size() +
                                sizeof(Value) +
                                data.size() +
                                2 * sizeof(uint64_t);

        ByteVector message(msg_size);
        size_t offset = 0;

        // Build message
        memcpy(message.data() + offset, &timestamp, sizeof(Timestamp));
        offset += sizeof(Timestamp);

        memcpy(message.data() + offset, serialized_sender.data(), serialized_sender.size());
        offset += serialized_sender.size();

        memcpy(message.data() + offset, serialized_receiver.data(), serialized_receiver.size());
        offset += serialized_receiver.size();

        memcpy(message.data() + offset, &value, sizeof(Value));
        offset += sizeof(Value);

        memcpy(message.data() + offset, data.data(), data.size());
        offset += data.size();

        memcpy(message.data() + offset, &nonce, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        memcpy(message.data() + offset, &gas_cost, sizeof(uint64_t));

        // Verify signature
        Blake3Hash hasher;
        hasher.update(message.data(), message.size());
        auto hash = hasher.finalize();

        return std::equal(hash.begin(), hash.end(), signature.begin());
    }


} // namespace quids::blockchain