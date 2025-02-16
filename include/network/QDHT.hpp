#pragma once

#include "network/QDHTConstants.hpp"
#include <memory>
#include <vector>
#include <array>
#include <string>
#include <chrono>
#include <optional>
#include <boost/asio.hpp>
#include "quantum/QuantumState.hpp"
#include "quantum/QKDSystem.hpp"
#include "quantum/QuantumConsensus.hpp"

namespace quids::network {

// Forward declarations
namespace quantum {
    class QKDSystem;
    class QuantumConsensus;
    enum class QuantumSecurityLevel {
        HIGH,
        MEDIUM,
        LOW
    };
}

// Forward declarations
class QDHTNode;
class QDHTBucket;
class QDHTRoutingTable;
struct QuantumRoutingTree;

// Message type enum
enum class MessageType {
    PING,
    PONG,
    STORE,
    STORE_ACK,
    FIND_VALUE,
    VALUE_RESPONSE,
    NODES_RESPONSE
};

// Internal message structures
struct Message {
    MessageType type;
    std::vector<uint8_t> data;
};

struct FindValueResponse {
    bool found;
    std::vector<uint8_t> value;
    std::vector<QNodeIdentity> closest_nodes;
};

// Quantum-secure node identity
struct QNodeIdentity {
    std::array<uint8_t, QDHT_ID_LENGTH / 8> id;
    std::vector<uint8_t> public_key;
    std::string address;
    uint16_t port{0};
    std::chrono::steady_clock::time_point last_seen;
    bool is_validator{false};
    ::quids::quantum::QuantumState quantum_state;

    static std::array<uint8_t, QDHT_ID_LENGTH / 8> distance(const QNodeIdentity& a, const QNodeIdentity& b);
    static QNodeIdentity generate();
    bool verify(const std::vector<uint8_t>& signature) const;
};

// Internal lookup state
struct LookupState {
    QNodeIdentity target;
    std::vector<QNodeIdentity> visited;
    std::vector<QNodeIdentity> pending;
};

// Quantum-enhanced bucket metrics
struct QBucketMetrics {
    double entanglement_factor{0.0};
    double quantum_entropy{0.0};
    double coherence_level{0.0};
    double routing_efficiency{0.0};
    ::quids::quantum::QuantumState quantum_state;
};

// Quantum routing tree structure
struct QuantumRoutingTree {
    ::quids::quantum::QuantumState state;
    std::vector<std::unique_ptr<QuantumRoutingTree>> children;
};

// Quantum-enhanced bucket
class QDHTBucket {
public:
    QDHTBucket();
    ~QDHTBucket();

    bool add_node(const QNodeIdentity& node);
    bool remove_node(const QNodeIdentity& node);
    std::vector<QNodeIdentity> get_closest_nodes(
        const QNodeIdentity& target,
        size_t count = QDHT_K
    );
    bool should_split() const;
    std::pair<std::unique_ptr<QDHTBucket>, std::unique_ptr<QDHTBucket>> split();
    void refresh();

    const QBucketMetrics& get_metrics() const { return metrics_; }
    const std::vector<QNodeIdentity>& get_nodes() const { return nodes_; }

private:
    std::vector<QNodeIdentity> nodes_;
    std::chrono::steady_clock::time_point last_updated_;
    size_t prefix_length_{0};
    std::array<uint8_t, QDHT_ID_LENGTH / 8> prefix_;
    QBucketMetrics metrics_;
    std::unique_ptr<QuantumRoutingTree> routing_tree_;

    void initialize_quantum_state();
    void update_metrics();
    bool verify_prefix_match(const QNodeIdentity& node) const;
    double calculate_distance(const std::array<uint8_t, QDHT_ID_LENGTH / 8>& a,
                            const std::array<uint8_t, QDHT_ID_LENGTH / 8>& b) const;
    double calculate_entanglement_factor() const;
    double calculate_coherence_level() const;
};

// Quantum-enhanced routing table
class QDHTRoutingTable {
public:
    explicit QDHTRoutingTable(const QNodeIdentity& self);
    ~QDHTRoutingTable();

    // Quantum-optimized node management
    bool add_node(const QNodeIdentity& node);
    bool remove_node(const QNodeIdentity& node);

    // Quantum-enhanced node search
    std::vector<QNodeIdentity> get_closest_nodes(
        const QNodeIdentity& target,
        size_t count = QDHT_K
    );

    // Quantum state maintenance
    void refresh();

    // Quantum metrics
    struct RoutingMetrics {
        double quantum_efficiency{0.0};
        double entanglement_density{0.0};
        double routing_coherence{0.0};
    };

    RoutingMetrics get_metrics() const;

private:
    QNodeIdentity self_;
    std::vector<std::unique_ptr<QDHTBucket>> buckets_;
    ::quids::quantum::QuantumState table_state_;

    QDHTBucket* find_bucket(const QNodeIdentity& node);
    void split_bucket(size_t index);
    void update_quantum_state();
};

// Main QDHT node with quantum enhancements
class QDHTNode {
public:
    explicit QDHTNode(boost::asio::io_context& io_context, uint16_t port);
    ~QDHTNode();

    // Node lifecycle
    void start();
    void stop();

    // Quantum-enhanced DHT operations
    std::vector<QNodeIdentity> find_node(const QNodeIdentity& target);
    bool store_value(
        const std::array<uint8_t, QDHT_ID_LENGTH / 8>& key,
        const std::vector<uint8_t>& value,
        quantum::QuantumSecurityLevel security_level = quantum::QuantumSecurityLevel::HIGH
    );
    std::optional<std::vector<uint8_t>> find_value(
        const std::array<uint8_t, QDHT_ID_LENGTH / 8>& key
    );

    // Quantum state maintenance
    void refresh_buckets();
    void bootstrap(const std::vector<std::pair<std::string, uint16_t>>& bootstrap_nodes);

private:
    boost::asio::io_context& io_context_;
    QNodeIdentity identity_;
    std::unique_ptr<QDHTRoutingTable> routing_table_;
    std::shared_ptr<quantum::QKDSystem> qkd_system_;
    std::shared_ptr<quantum::QuantumConsensus> consensus_;

    // Quantum-enhanced network operations
    void handle_find_node(const QNodeIdentity& sender, const QNodeIdentity& target);
    void handle_store_value(
        const QNodeIdentity& sender,
        const std::array<uint8_t, QDHT_ID_LENGTH / 8>& key,
        const std::vector<uint8_t>& value
    );
    void maintain_quantum_state();
    void process_quantum_messages();
};

} // namespace quids::network