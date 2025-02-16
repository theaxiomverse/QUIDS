#include "network/QDHT.hpp"
#include "network/QDHTConstants.hpp"
#include "quantum/QuantumCrypto.hpp"
#include "quantum/QuantumState.hpp"
#include "quantum/QKDSystem.hpp"
#include "quantum/QuantumConsensus.hpp"
#include <spdlog/spdlog.h>
#include <boost/asio.hpp>
#include <random>
#include <algorithm>
#include <cstring>
#include <queue>
#include <bitset>
#include <future>
#include <unordered_map>
#include <iostream>
#include <chrono>
#include <Eigen/Dense>
#include <numeric>
#include <cmath>
#include <map>
#include <execution>
#include <immintrin.h>

using std::make_unique;
using std::unique_ptr;
using std::vector;
using std::array;
using std::chrono::steady_clock;
using quids::quantum::QuantumState;

namespace quids::network {

// Helper function declarations
bool get_bit(const array<uint8_t, QDHT_ID_LENGTH / 8>& id, size_t index) {
    return (id[index / 8] >> (index % 8)) & 1;
}

// QDHTBucket method implementations
QDHTBucket::QDHTBucket()
    : prefix_length_(0),
      metrics_({0.0, 0.0, 0.0, 0.0, QuantumState(QDHT_ID_LENGTH)}),
      routing_tree_(make_unique<QuantumRoutingTree>()),
      last_updated_(steady_clock::now()) {
    prefix_.fill(0);
    initialize_quantum_state();
}

QDHTBucket::~QDHTBucket() = default;

void QDHTBucket::initialize_quantum_state() {
    QuantumState state(QDHT_ID_LENGTH);
    for (const auto& node : nodes_) {
        auto combined = state.getStateVector() + node.quantum_state.getStateVector();
        combined.normalize();
        state = QuantumState(::std::move(combined));
    }
    routing_tree_->state = ::std::move(state);
}

bool QDHTBucket::add_node(const QNodeIdentity& node) {
    auto it = std::find_if(nodes_.begin(), nodes_.end(),
        [&node](const QNodeIdentity& n) {
            for (size_t i = 0; i < n.id.size(); ++i) {
                if (n.id[i] != node.id[i]) return false;
            }
            return true;
        });
    
    if (it != nodes_.end()) {
        *it = node;
        update_metrics();
        return true;
    }
    
    if (nodes_.size() >= QDHT_K) {
        return false;
    }
    
    nodes_.push_back(node);
    update_metrics();
    return true;
}

bool QDHTBucket::remove_node(const QNodeIdentity& node) {
    auto it = std::find_if(nodes_.begin(), nodes_.end(),
        [&node](const QNodeIdentity& n) {
            for (size_t i = 0; i < n.id.size(); ++i) {
                if (n.id[i] != node.id[i]) return false;
            }
            return true;
        });
    
    if (it != nodes_.end()) {
        nodes_.erase(it);
        this->update_metrics();
        return true;
    }
    
    return false;
}

std::vector<QNodeIdentity> QDHTBucket::get_closest_nodes(
        const QNodeIdentity& target,
        size_t count) {
    std::vector<std::pair<QNodeIdentity, double>> distances;
    distances.reserve(nodes_.size());
    
    for (const auto& node : nodes_) {
        double distance = calculate_distance(node.id, target.id);
        distances.emplace_back(node, distance);
    }
    
    std::sort(distances.begin(), distances.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    
    std::vector<QNodeIdentity> result;
    result.reserve(std::min(count, distances.size()));
    
    for (size_t i = 0; i < std::min(count, distances.size()); ++i) {
        result.push_back(distances[i].first);
    }
    
    return result;
}

bool QDHTBucket::should_split() const {
    return nodes_.size() >= QDHT_K && prefix_length_ < QDHT_ID_LENGTH;
}

std::pair<std::unique_ptr<QDHTBucket>, std::unique_ptr<QDHTBucket>> QDHTBucket::split() {
    auto left = std::make_unique<QDHTBucket>();
    auto right = std::make_unique<QDHTBucket>();
    
    left->prefix_length_ = prefix_length_ + 1;
    right->prefix_length_ = prefix_length_ + 1;
    
    left->prefix_ = prefix_;
    right->prefix_ = prefix_;
    if (prefix_length_ < QDHT_ID_LENGTH) {
        right->prefix_[prefix_length_ / 8] |= (1 << (prefix_length_ % 8));
    }
    
    for (const auto& node : nodes_) {
        if (get_bit(node.id, prefix_length_)) {
            right->add_node(node);
        } else {
            left->add_node(node);
        }
    }
    
    return std::make_pair(::std::move(left), ::std::move(right));
}

void QDHTBucket::refresh() {
    auto now = std::chrono::steady_clock::now();
    nodes_.erase(
        std::remove_if(nodes_.begin(), nodes_.end(),
            [&now](const QNodeIdentity& node) {
                return (now - node.last_seen) > std::chrono::minutes(30);
            }),
        nodes_.end());
    
    update_metrics();
}

double QDHTBucket::calculate_distance(
    const array<uint8_t, QDHT_ID_LENGTH / 8>& a,
    const array<uint8_t, QDHT_ID_LENGTH / 8>& b) const {
    double distance = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        unsigned char x = a[i] ^ b[i];
        while (x) {
            distance += x & 1;
            x >>= 1;
        }
    }
    return distance;
}

void QDHTBucket::update_metrics() {
    metrics_.entanglement_factor = calculate_entanglement_factor();
    metrics_.quantum_entropy = calculate_coherence_level();
    metrics_.coherence_level = calculate_coherence_level();
    metrics_.routing_efficiency = 1.0 - (static_cast<double>(nodes_.size()) / QDHT_K);
    
    auto combined = metrics_.quantum_state.getStateVector() + routing_tree_->state.getStateVector();
    combined.normalize();
    metrics_.quantum_state = QuantumState(::std::move(combined));
}

double QDHTBucket::calculate_entanglement_factor() const {
    if (nodes_.empty()) return 0.0;
    
    double total_entanglement = 0.0;
    size_t num_pairs = 0;
    
    for (size_t i = 0; i < nodes_.size(); ++i) {
        for (size_t j = i + 1; j < nodes_.size(); ++j) {
            auto overlap = (nodes_[i].quantum_state.getStateVector().adjoint() * 
                          nodes_[j].quantum_state.getStateVector()).norm();
            total_entanglement += overlap;
            num_pairs++;
        }
    }
    
    return num_pairs > 0 ? total_entanglement / num_pairs : 0.0;
}

double QDHTBucket::calculate_coherence_level() const {
    if (nodes_.empty()) return 0.0;
    
    double total_coherence = 0.0;
    for (const auto& node : nodes_) {
        auto state_vector = node.quantum_state.getStateVector();
        total_coherence += (state_vector.adjoint() * state_vector).norm();
    }
    
    return total_coherence / nodes_.size();
}

bool QDHTBucket::verify_prefix_match(const QNodeIdentity& node) const {
    for (size_t i = 0; i < prefix_length_; ++i) {
        if (get_bit(node.id, i) != get_bit(prefix_, i)) {
            return false;
        }
    }
    return true;
}

} // namespace quids::network