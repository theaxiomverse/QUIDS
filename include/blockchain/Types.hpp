#ifndef QUIDS_BLOCKCHAIN_TYPES_HPP
#define QUIDS_BLOCKCHAIN_TYPES_HPP

#include <array>
#include <vector>
#include <memory>
#include <optional>
#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <cstdint>
#include <type_traits>
#include <utility>

// STL type aliases
namespace quids::blockchain {
    using ByteArray = ::std::array<uint8_t, 32>;
    using ByteVector = ::std::vector<uint8_t>;
    using Hash = ByteArray;
    using Timestamp = ::std::chrono::system_clock::time_point;
    using BlockNumber = uint64_t;
    using TransactionId = ::std::string;
    using BlockId = ::std::string;
    using GasPrice = uint64_t;
    using GasLimit = uint64_t;
    using Nonce = uint64_t;
    using Value = uint64_t;
    using Address = ::std::string;
    using Data = ByteVector;
    using Signature = ByteArray;
    using PublicKey = ByteArray;
    using PrivateKey = ByteArray;
}

// Forward declarations
namespace quids {
    namespace memory {
        template<typename T> class MemoryPool;
    }
    namespace state {
        class LockFreeStateManager;
    }
    namespace consensus {
        class QuantumConsensusModule;
    }
    namespace rl {
        class QuantumRLAgent;
    }
    namespace crypto {
        class Blake3Hash;
    }
    namespace neural {
        class QuantumPolicyNetwork;
        class QuantumValueNetwork;
    }
    namespace quantum {
        class QuantumState;
        class QuantumCircuit;
        struct QuantumCircuitConfig;
    }
}

namespace quids::blockchain {

// Forward declarations
class Block;
class AIBlock;
class Transaction;
struct AIBlockConfig;
struct AIMetrics;

// Common type aliases
using TimePoint = ::std::chrono::system_clock::time_point;
using Duration = ::std::chrono::system_clock::duration;
using MemoryPoolPtr = std::unique_ptr<memory::MemoryPool<Transaction>>;
using StateManagerPtr = std::unique_ptr<state::LockFreeStateManager>;
using ConsensusPtr = std::unique_ptr<consensus::QuantumConsensusModule>;
using RLAgentPtr = std::unique_ptr<rl::QuantumRLAgent>;
using PolicyNetworkPtr = std::unique_ptr<neural::QuantumPolicyNetwork>;
using ValueNetworkPtr = std::unique_ptr<neural::QuantumValueNetwork>;
using QuantumStatePtr = std::unique_ptr<quantum::QuantumState>;
using QuantumCircuitPtr = std::unique_ptr<quantum::QuantumCircuit>;

// Type aliases
template<typename T>
using Vector = std::vector<T>;

template<typename T>
using Optional = std::optional<T>;

template<typename T>
using Atomic = std::atomic<T>;

template<typename T, size_t N>
using Array = std::array<T, N>;

using Mutex = std::mutex;
using string = std::string;

// Component type aliases
using MemoryPool = memory::MemoryPool<Transaction>;
using StateManager = state::LockFreeStateManager;
using ConsensusModule = consensus::QuantumConsensusModule;
using RLAgent = rl::QuantumRLAgent;
using QuantumState = quantum::QuantumState;
using QuantumCircuit = quantum::QuantumCircuit;
using QuantumCircuitConfig = quantum::QuantumCircuitConfig;
using QuantumPolicyNetwork = neural::QuantumPolicyNetwork;
using QuantumValueNetwork = neural::QuantumValueNetwork;
using Blake3Hash = crypto::Blake3Hash;

// Constants
constexpr size_t HASH_SIZE = 32;
constexpr size_t MAX_TRANSACTIONS = 1024;
constexpr size_t DEFAULT_BATCH_SIZE = 128;

// Utility functions
template<typename T>
inline T min(T a, T b) {
    return std::min(a, b);
}

template<typename T>
inline T abs(T x) {
    return std::abs(x);
}

template<typename InputIt, typename OutputIt>
inline OutputIt copy_n(InputIt first, size_t count, OutputIt result) {
    return std::copy_n(first, count, result);
}

template<typename InputIt, typename Pred>
inline bool all_of(InputIt first, InputIt last, Pred pred) {
    return std::all_of(first, last, pred);
}

} // namespace quids::blockchain

#endif // QUIDS_BLOCKCHAIN_TYPES_HPP 