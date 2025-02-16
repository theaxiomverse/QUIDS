#include "quantum/QuantumState.hpp"
#include <cmath>
#include <random>
#include <iostream>
#include <unordered_map>
#include <shared_mutex>

namespace quids {
namespace quantum {

using namespace Eigen;

// Define the static member
std::unordered_map<size_t, QuantumState> QuantumState::state_cache_{};

// Add mutex for cache access
static std::shared_mutex cache_mutex_;

class QuantumState::Impl {
public:
    explicit Impl(size_t num_qubits)
        : num_qubits_(num_qubits),
          state_vector_(1 << num_qubits),
          entanglement_(1 << num_qubits, 1 << num_qubits),
          coherence_(0.0),
          entropy_(0.0),
          measurement_outcomes_() {
        // Initialize to |0...0> state
        state_vector_.setZero();
        state_vector_(0) = 1.0;
        generate_entanglement_matrix();
    }

    explicit Impl(const Eigen::VectorXcd& state_vector)
        : num_qubits_(static_cast<size_t>(std::log2(state_vector.size()))),
          state_vector_(state_vector),
          entanglement_(state_vector.size(), state_vector.size()),
          coherence_(0.0),
          entropy_(0.0),
          measurement_outcomes_() {
        // Verify state vector size is power of 2
        if ((state_vector.size() & (state_vector.size() - 1)) != 0) {
            throw std::invalid_argument("State vector size must be a power of 2");
        }
        generate_entanglement_matrix();
    }

    void generate_entanglement_matrix() {
        entanglement_ = state_vector_ * state_vector_.adjoint();
        calculate_coherence();
        calculate_entropy();
    }

    void calculate_coherence() {
        coherence_ = 0.0;
        for (Eigen::Index i = 0; i < state_vector_.size(); i++) {
            for (Eigen::Index j = 0; j < state_vector_.size(); j++) {
                if (i != j) {
                    coherence_ += std::abs(entanglement_(i, j));
                }
            }
        }
    }

    void calculate_entropy() {
        entropy_ = 0.0;
        for (Eigen::Index i = 0; i < state_vector_.size(); i++) {
            double p = std::norm(state_vector_[i]);
            if (p > 0) {
                entropy_ -= p * std::log2(p);
            }
        }
    }

    size_t num_qubits_;
    Eigen::VectorXcd state_vector_;
    Eigen::MatrixXcd entanglement_;
    double coherence_;
    double entropy_;
    std::vector<bool> measurement_outcomes_;

    Eigen::VectorXcd& get_state_vector() { return state_vector_; }
};

QuantumState::QuantumState() : impl_(std::make_unique<Impl>(1)) {}

QuantumState::QuantumState(size_t num_qubits)
    : impl_(std::make_unique<Impl>(num_qubits)) {}

QuantumState::QuantumState(const Eigen::VectorXcd& state_vector)
    : impl_(std::make_unique<Impl>(state_vector)) {}

QuantumState::QuantumState(const QuantumState& other)
    : impl_(std::make_unique<Impl>(*other.impl_)) {}

QuantumState::QuantumState(QuantumState&& other) noexcept = default;

QuantumState& QuantumState::operator=(const QuantumState& other) {
    if (this != &other) {
        impl_ = std::make_unique<Impl>(*other.impl_);
    }
    return *this;
}

QuantumState& QuantumState::operator=(QuantumState&& other) noexcept = default;

QuantumState::~QuantumState() = default;

size_t QuantumState::getNumQubits() const {
    return impl_->num_qubits_;
}

size_t QuantumState::size() const {
    return impl_->state_vector_.size();
}

void QuantumState::applySingleQubitGate(size_t qubit, const Eigen::Matrix2cd& gate) {
    if (qubit >= impl_->num_qubits_) {
        throw std::out_of_range("Qubit index out of range");
    }

    size_t n = 1ULL << impl_->num_qubits_;
    size_t mask = 1ULL << qubit;
    
    Eigen::VectorXcd new_state = impl_->state_vector_;
    
    for (size_t i = 0; i < n; i++) {
        if ((i & mask) == 0) {
            size_t i1 = i;
            size_t i2 = i | mask;
            
            std::complex<double> a = impl_->state_vector_(i1);
            std::complex<double> b = impl_->state_vector_(i2);
            
            new_state(i1) = gate(0,0) * a + gate(0,1) * b;
            new_state(i2) = gate(1,0) * a + gate(1,1) * b;
        }
    }
    
    impl_->state_vector_ = new_state;
}

void QuantumState::applyCNOT(size_t control, size_t target) {
    if (control >= impl_->num_qubits_ || target >= impl_->num_qubits_) {
        throw std::out_of_range("Qubit index out of range");
    }

    size_t n = 1ULL << impl_->num_qubits_;
    size_t control_mask = 1ULL << control;
    size_t target_mask = 1ULL << target;
    
    Eigen::VectorXcd new_state = impl_->state_vector_;
    
    for (size_t i = 0; i < n; i++) {
        if ((i & control_mask) != 0) {  // Control qubit is 1
            size_t j = i ^ target_mask;  // Flip target qubit
            std::swap(new_state(i), new_state(j));
        }
    }
    
    impl_->state_vector_ = new_state;
}

void QuantumState::applyPhase(size_t qubit, double angle) {
    Eigen::Matrix2cd phase_gate;
    phase_gate << 1.0, 0.0,
                  0.0, std::exp(std::complex<double>(0, angle));
    applySingleQubitGate(qubit, phase_gate);
}

void QuantumState::applyMeasurement(size_t qubit) {
    if (qubit >= impl_->num_qubits_) {
        throw std::out_of_range("Qubit index out of range");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    size_t n = 1ULL << impl_->num_qubits_;
    size_t mask = 1ULL << qubit;
    
    // Calculate probability of measuring |1>
    double prob_one = 0.0;
    for (size_t i = 0; i < n; i++) {
        if ((i & mask) != 0) {
            prob_one += std::norm(impl_->state_vector_(i));
        }
    }
    
    // Perform measurement
    bool result = dis(gen) < prob_one;
    impl_->measurement_outcomes_.push_back(result);
    
    // Project state
    double norm_factor = 1.0 / std::sqrt(result ? prob_one : (1.0 - prob_one));
    for (size_t i = 0; i < n; i++) {
        if (((i & mask) != 0) != result) {
            impl_->state_vector_(i) = 0.0;
        } else {
            impl_->state_vector_(i) *= norm_factor;
        }
    }
}

std::vector<bool> QuantumState::getMeasurementOutcomes() const {
    return impl_->measurement_outcomes_;
}

void QuantumState::normalize() {
    impl_->state_vector_.normalize();
}

const Eigen::VectorXcd& QuantumState::normalizedVector() const {
    return impl_->state_vector_;
}

const Eigen::MatrixXcd& QuantumState::entanglementMatrix() const {
    return impl_->entanglement_;
}

double QuantumState::getCoherence() const {
    return impl_->coherence_;
}

double QuantumState::getEntropy() const {
    return impl_->entropy_;
}

void QuantumState::generateEntanglementMatrix() {
    impl_->generate_entanglement_matrix();
}

void QuantumState::validateState() const {
    if (impl_->state_vector_.size() == 0) {
        throw std::runtime_error("Invalid quantum state: empty state vector");
    }
}

Eigen::MatrixXcd QuantumState::createSingleQubitGate(
    const Eigen::MatrixXcd& gate,
    size_t target_qubit) const {
    size_t n = impl_->num_qubits_;
    if (target_qubit >= n) {
        throw std::out_of_range("Target qubit index out of range");
    }
    
    Eigen::MatrixXcd result = Eigen::MatrixXcd::Identity(impl_->state_vector_.size(), impl_->state_vector_.size());
    size_t step = 1ULL << target_qubit;
    
    for (Eigen::Index i = 0; i < impl_->state_vector_.size(); i += 2 * step) {
        for (size_t j = 0; j < step; j++) {
            size_t b0 = i + j;
            size_t b1 = b0 + step;
            
            result(b0, b0) = gate(0, 0);
            result(b0, b1) = gate(0, 1);
            result(b1, b0) = gate(1, 0);
            result(b1, b1) = gate(1, 1);
        }
    }
    
    return result;
}

void QuantumState::applyHadamard(size_t qubit) {
    if (qubit >= impl_->num_qubits_) {
        throw std::out_of_range("Target qubit index out of range");
    }
    
    Eigen::Matrix2cd H;
    H << 1.0/std::sqrt(2.0), 1.0/std::sqrt(2.0),
         1.0/std::sqrt(2.0), -1.0/std::sqrt(2.0);
    
    applySingleQubitGate(qubit, H);
}

Eigen::MatrixXcd QuantumState::generateEntanglement() const {
    return impl_->entanglement_;
}

std::vector<Eigen::MatrixXcd> QuantumState::createLayers() const {
    std::vector<Eigen::MatrixXcd> layers;
    
    for (size_t i = 0; i < impl_->num_qubits_; i++) {
        Eigen::Matrix2cd H;
        H << 1.0/std::sqrt(2.0), 1.0/std::sqrt(2.0),
             1.0/std::sqrt(2.0), -1.0/std::sqrt(2.0);
        layers.push_back(createSingleQubitGate(H, i));
    }
    
    return layers;
}

double QuantumState::calculateCoherence() const {
    return impl_->coherence_;
}

double QuantumState::calculateEntropy() const {
    return impl_->entropy_;
}

const Eigen::VectorXcd& QuantumState::getStateVector() const {
    return impl_->state_vector_;
}

void QuantumState::applyGateOptimized(const Eigen::MatrixXcd& gate) {
    auto& state_vector = impl_->get_state_vector();
    
    // Create temporary vector for result
    Eigen::VectorXcd result = gate * state_vector;
    
    // Copy back result
    #pragma omp parallel for simd
    for (Eigen::Index i = 0; i < state_vector.size(); ++i) {
        state_vector(i) = result(i);
    }
    
    validateState();
}

void QuantumState::setAmplitude(size_t index, const std::complex<double>& value) {
    if (static_cast<Eigen::Index>(index) >= impl_->state_vector_.size()) {
        throw std::out_of_range("Amplitude index out of range");
    }
    impl_->state_vector_(index) = value;
}

bool QuantumState::isValid() const {
    try {
        validateState();
        // Check if state vector is normalized (within tolerance)
        double norm = impl_->state_vector_.norm();
        return std::abs(norm - 1.0) < 1e-10;
    } catch (...) {
        return false;
    }
}

std::complex<double> QuantumState::getAmplitude(size_t index) const {
    if (!impl_ || static_cast<Eigen::Index>(index) >= impl_->state_vector_.size()) {
        throw std::out_of_range("Invalid amplitude index");
    }
    return impl_->state_vector_(index);
}

void QuantumState::prepareState() {
    if (!impl_) {
        throw std::runtime_error("Quantum state not properly initialized");
    }
    
    // Initialize to |0⟩ state
    impl_->state_vector_.setZero();
    impl_->state_vector_(0) = 1.0;
    
    // Apply initial quantum circuit operations
    try {
        // Apply Hadamard gates to create superposition
        for (size_t i = 0; i < impl_->num_qubits_; ++i) {
            applyHadamard(i);
        }
        
        // Generate entanglement between adjacent qubits
        for (size_t i = 0; i < impl_->num_qubits_ - 1; ++i) {
            applyCNOT(i, i + 1);
        }
        
        // Apply phase rotations
        for (size_t i = 0; i < impl_->num_qubits_; ++i) {
            applyPhase(i, M_PI / 4.0);  // π/4 rotation
        }
        
        // Normalize the final state
        normalize();
        
        // Update entanglement matrix and measurements
        generateEntanglementMatrix();
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to prepare quantum state: ") + e.what());
    }
}

} // namespace quantum
} // namespace quids 