#include "blockchain/Transaction.hpp"
#include "crypto/blake3/Blake3Hash.hpp"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/core.h>
#include <openssl/core_names.h>
#include <openssl/provider.h>
#include <openssl/params.h>
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <openssl/sha.h>
#include <blake3.h>
#include <spdlog/spdlog.h>
#include <cstring>
#include <memory>

namespace {
    // Thread-safe OpenSSL error checking
std::string get_openssl_error() {
    char err_buf[256];
    unsigned long err = ERR_get_error();
    ERR_error_string_n(err, err_buf, sizeof(err_buf));
    return std::string(err_buf); // #include <string>
}

    // RAII wrapper for OpenSSL initialization
    class OpenSSLGuard {
    public:
        OpenSSLGuard() {
            OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CONFIG, nullptr);
            legacy_provider_ = OSSL_PROVIDER_load(nullptr, "legacy");
            default_provider_ = OSSL_PROVIDER_load(nullptr, "default");
        }

        ~OpenSSLGuard() {
            if (legacy_provider_) OSSL_PROVIDER_unload(legacy_provider_);
            if (default_provider_) OSSL_PROVIDER_unload(default_provider_);
            OPENSSL_cleanup();
        }

    private:
        OSSL_PROVIDER* legacy_provider_;
        OSSL_PROVIDER* default_provider_;
    };

    // Static initialization
    static const OpenSSLGuard openssl_guard;
}

namespace quids::blockchain {

void Transaction::serialize(ByteVector& out) const {
    // Reserve space for all fields
    out.reserve(sizeof(Timestamp) + 2 * sizeof(Address) + sizeof(Value) + 
                data.size() + sizeof(Signature) + 2 * sizeof(uint64_t));

    // Serialize all fields
    const char* timestamp_ptr = reinterpret_cast<const char*>(&timestamp);
    out.insert(out.end(), timestamp_ptr, timestamp_ptr + sizeof(Timestamp));

    out.insert(out.end(), sender.begin(), sender.end());
    out.insert(out.end(), receiver.begin(), receiver.end());

    const char* value_ptr = reinterpret_cast<const char*>(&value);
    out.insert(out.end(), value_ptr, value_ptr + sizeof(Value));

    out.insert(out.end(), data.begin(), data.end());
    out.insert(out.end(), signature.begin(), signature.end());

    const char* nonce_ptr = reinterpret_cast<const char*>(&nonce);
    out.insert(out.end(), nonce_ptr, nonce_ptr + sizeof(uint64_t));

    const char* gas_cost_ptr = reinterpret_cast<const char*>(&gas_cost);
    out.insert(out.end(), gas_cost_ptr, gas_cost_ptr + sizeof(uint64_t));
}

bool Transaction::deserialize(const ByteVector& data) {
    if (data.size() < sizeof(Timestamp) + 2 * sizeof(Address) + sizeof(Value) + 
                      sizeof(Signature) + 2 * sizeof(uint64_t)) {
        return false;
    }

    size_t offset = 0;

    // Deserialize timestamp
    std::memcpy(&timestamp, data.data() + offset, sizeof(Timestamp));
    offset += sizeof(Timestamp);

    // Deserialize addresses
    std::copy(data.begin() + offset, data.begin() + offset + sizeof(Address), sender.begin());
    offset += sizeof(Address);
    std::copy(data.begin() + offset, data.begin() + offset + sizeof(Address), receiver.begin());
    offset += sizeof(Address);

    // Deserialize value
    std::memcpy(&value, data.data() + offset, sizeof(Value));
    offset += sizeof(Value);

    // Deserialize data
    size_t data_size = data.size() - offset - sizeof(Signature) - 2 * sizeof(uint64_t);
    ByteVector temp_data;
    temp_data.resize(data_size);
    std::copy(data.begin() + offset, data.begin() + offset + data_size, temp_data.begin());
    this->data = temp_data;
    offset += data_size;

    // Deserialize signature
    std::copy(data.begin() + offset, data.begin() + offset + sizeof(Signature), signature.begin());
    offset += sizeof(Signature);

    // Deserialize nonce and gas cost
    std::memcpy(&nonce, data.data() + offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    std::memcpy(&gas_cost, data.data() + offset, sizeof(uint64_t));

    return true;
}

std::vector<uint8_t> Transaction::computeHash() const {
    ByteVector serialized;
    serialize(serialized);
    
    Blake3Hash hasher;
    hasher.update(serialized.data(), serialized.size());
    return hasher.finalize();
}

bool Transaction::verify() const {
    if (sender.empty() || receiver.empty()) {
        return false;
    }

    // Verify signature
    ByteVector message;
    message.reserve(sizeof(Timestamp) + 2 * sizeof(Address) + sizeof(Value) + 
                   data.size() + 2 * sizeof(uint64_t));

    const char* timestamp_ptr = reinterpret_cast<const char*>(&timestamp);
    message.insert(message.end(), timestamp_ptr, timestamp_ptr + sizeof(Timestamp));
    message.insert(message.end(), sender.begin(), sender.end());
    message.insert(message.end(), receiver.begin(), receiver.end());

    const char* value_ptr = reinterpret_cast<const char*>(&value);
    message.insert(message.end(), value_ptr, value_ptr + sizeof(Value));
    message.insert(message.end(), data.begin(), data.end());

    const char* nonce_ptr = reinterpret_cast<const char*>(&nonce);
    message.insert(message.end(), nonce_ptr, nonce_ptr + sizeof(uint64_t));

    const char* gas_cost_ptr = reinterpret_cast<const char*>(&gas_cost);
    message.insert(message.end(), gas_cost_ptr, gas_cost_ptr + sizeof(uint64_t));

    Blake3Hash hasher;
    hasher.update(message.data(), message.size());
    auto hash = hasher.finalize();
    return ByteVector(hash.begin(), hash.end()) == ByteVector(signature.begin(), signature.end());
}

::std::string Transaction::toString() const {
    std::stringstream ss;
    ss << "Transaction{"
       << "sender=" << sender
       << ", receiver=" << receiver
       << ", value=" << value
       << ", nonce=" << nonce
       << ", gas_cost=" << gas_cost
       << ", data_size=" << data.size()
       << "}";
    return ss.str();
}

} // namespace quids::blockchain 