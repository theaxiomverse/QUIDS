#pragma once
#include "quantum/QuantumState.hpp"
#include "quantum/QuantumGates.hpp"
#include "quantum/QuantumOperations.hpp"
#include "quantum/QuantumCircuit.hpp"
#include "quantum/QuantumConsensus.hpp"



#include <Eigen/Dense>
#include <random>
#include <omp.h>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>

namespace quids {
namespace quantum {

class QuantumState;

using namespace Eigen;

namespace {
    // Helper functions
    Matrix2cd createHadamardGate() {
        Matrix2cd H;
        H << 1.0/std::sqrt(2.0), 1.0/std::sqrt(2.0),
             1.0/std::sqrt(2.0), -1.0/std::sqrt(2.0);
        return H;
    }

    Matrix2cd createPhaseGate(double angle) {
        Matrix2cd P;
        P << 1.0, 0.0,
             0.0, std::exp(std::complex<double>(0.0, angle));
        return P;
    }
}

// Implementation class
class QuantumState::Impl {
public:
    explicit Impl(std::size_t num_qubits) 
        : num_qubits_(num_qubits),
          state_vector_(1ULL << num_qubits),
          entanglement_(1ULL << num_qubits, 1ULL << num_qubits),
          coherence_(0.0),
          entropy_(0.0) {
        state_vector_.setZero();
        state_vector_(0) = 1.0;
        generateEntanglementMatrix();
    }

    // Member variables
    std::size_t num_qubits_;
    VectorXcd state_vector_;
    MatrixXcd entanglement_;
    std::vector<bool> measurement_outcomes_;
    std::vector<double> features_;
    double coherence_;
    double entropy_;

    // Implementation methods
    void applyHadamard(std::size_t qubit) {
        Matrix2cd H = createHadamardGate();
        applySingleQubitGate(qubit, H);
    }

    void applyPhase(std::size_t qubit, double angle) {
        Matrix2cd P = createPhaseGate(angle);
        applySingleQubitGate(qubit, P);
    }

    void applyCNOT(std::size_t control, std::size_t target) {
        if (control >= num_qubits_ || target >= num_qubits_) {
            throw std::out_of_range("Qubit index out of range");
        }

        std::size_t n = 1ULL << num_qubits_;
        std::size_t control_mask = 1ULL << control;
        std::size_t target_mask = 1ULL << target;
        
        VectorXcd new_state = state_vector_;
        
        for (std::size_t i = 0; i < n; i++) {
            if ((i & control_mask) != 0) {  // Control qubit is 1
                std::size_t j = i ^ target_mask;  // Flip target qubit
                std::swap(new_state(i), new_state(j));
            }
        }
        
        state_vector_ = new_state;
    }

    void applySingleQubitGate(std::size_t qubit, const Matrix2cd& gate) {
        if (qubit >= num_qubits_) {
            throw std::out_of_range("Qubit index out of range");
        }

        std::size_t n = 1ULL << num_qubits_;
        std::size_t mask = 1ULL << qubit;

        VectorXcd new_state = state_vector_;

        for (std::size_t i = 0; i < n; i++) {
            if ((i & mask) == 0) {
                std::size_t i1 = i;
                std::size_t i2 = i | mask;

                std::complex<double> a = state_vector_(i1);
                std::complex<double> b = state_vector_(i2);

                new_state(i1) = gate(0,0) * a + gate(0,1) * b;
                new_state(i2) = gate(1,0) * a + gate(1,1) * b;
            }
        }

        state_vector_ = new_state;
    }

    void applyMeasurement(std::size_t qubit) {
        if (qubit >= num_qubits_) {
            throw std::out_of_range("Qubit index out of range");
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        std::size_t n = 1ULL << num_qubits_;
        std::size_t mask = 1ULL << qubit;

        double prob_one = 0.0;
        for (std::size_t i = 0; i < n; i++) {
            if ((i & mask) != 0) {
                prob_one += std::abs(state_vector_(i)) * std::abs(state_vector_(i));
            }
        }

        bool result = dis(gen) < prob_one;
        measurement_outcomes_.push_back(result);

        double norm_factor = 1.0 / std::sqrt(result ? prob_one : (1.0 - prob_one));
        for (std::size_t i = 0; i < n; i++) {
            if (((i & mask) != 0) != result) {
                state_vector_(i) = 0.0;
            } else {
                state_vector_(i) *= norm_factor;
            }
        }
    }

    void applyGateOptimized(const MatrixXcd& gate) {
        state_vector_ = gate * state_vector_;
    }

    void prepareState() {
        state_vector_.setZero();
        state_vector_(0) = 1.0;
        
        for (std::size_t i = 0; i < num_qubits_; ++i) {
            applyHadamard(i);
        }
    }

    const VectorXcd& normalizedVector() const noexcept {
        return state_vector_.normalized();
    }

    std::size_t getNumQubits() const noexcept {
        return num_qubits_;
    }

    std::size_t size() const noexcept {
        return state_vector_.size();
    }

    void normalize() {
        state_vector_.normalize();
    }

    bool isValid() const noexcept {
        try {
            validateState();
            return std::abs(state_vector_.norm() - 1.0) < 1e-10;
        } catch (...) {
            return false;
        }
    }

    const VectorXcd& getStateVector() const noexcept {
        return state_vector_;
    }

    std::vector<bool> getMeasurementOutcomes() const {
        return measurement_outcomes_;
    }

    std::complex<double> getAmplitude(std::size_t index) const {
        if (static_cast<Eigen::Index>(index) >= state_vector_.size()) {
            throw std::out_of_range("Invalid amplitude index");
        }
        return state_vector_(index);
    }

    void setAmplitude(std::size_t index, const std::complex<double>& value) {
        if (static_cast<Eigen::Index>(index) >= state_vector_.size()) {
            throw std::out_of_range("Amplitude index out of range");
        }
        state_vector_(index) = value;
    }

    double getCoherence() const noexcept {
        return coherence_;
    }

    double getEntropy() const noexcept {
        return entropy_;
    }

    const MatrixXcd& entanglementMatrix() const {
        return entanglement_;
    }

    MatrixXcd generateEntanglement() const {
        return entanglement_;
    }

    std::vector<MatrixXcd> createLayers() const {
        return {entanglement_};
    }

    double calculateCoherence() const noexcept {
        return coherence_;
    }

    double calculateEntropy() const noexcept {
        return entropy_;
    }

    void encode(std::vector<double> features) {
        features_ = std::move(features);
        const std::size_t n = 1ULL << num_qubits_;
        const std::size_t feature_size = features_.size();
        
        state_vector_.setZero();
        for (std::size_t i = 0; i < std::min(n, feature_size); ++i) {
            state_vector_(i) = std::complex<double>(features_[i], 0.0);
        }
        normalize();
    }

    const std::vector<double>& getFeatures() const {
        return features_;
    }

    void generateEntanglementMatrix() {
        entanglement_ = MatrixXcd::Identity(size(), size());
    }

    void validateState() const {
        if (state_vector_.size() == 0) {
            throw std::runtime_error("Invalid quantum state: empty state vector");
        }
    }

    MatrixXcd createSingleQubitGate(const MatrixXcd& gate, std::size_t target_qubit) const {
        if (target_qubit >= num_qubits_) {
            throw std::out_of_range("Target qubit index out of range");
        }
        return gate;
    }
};

// Static member initialization
std::unordered_map<std::size_t, QuantumState> QuantumState::state_cache_{};
static std::shared_mutex cache_mutex_;

// Constructor implementations
QuantumState::QuantumState() : impl_(std::make_unique<Impl>(1)) {}
QuantumState::QuantumState(std::size_t num_qubits) : impl_(std::make_unique<Impl>(num_qubits)) {}
QuantumState::QuantumState(const VectorXcd& state_vector) 
    : impl_(std::make_unique<Impl>(static_cast<std::size_t>(std::log2(state_vector.size())))) {
    impl_->state_vector_ = state_vector;
    impl_->normalize();
}



// Conversion operators
QuantumState::operator const StateVector&() const noexcept {
    return impl_->getStateVector();
}

QuantumState::operator StateVector&() noexcept {
    return impl_->state_vector_;
}

// Copy and move operations
QuantumState::QuantumState(const QuantumState& other) : impl_(std::make_unique<Impl>(*other.impl_)) {}
QuantumState::QuantumState(QuantumState&&) noexcept = default;
QuantumState& QuantumState::operator=(const QuantumState& other) {
    if (this != &other) {
        impl_ = std::make_unique<Impl>(*other.impl_);
    }
    return *this;
}
QuantumState& QuantumState::operator=(QuantumState&&) noexcept = default;
QuantumState::~QuantumState() = default;

class QuantumState;

// Method implementations
void QuantumState::applyHadamard(std::size_t qubit) {
    impl_->applyHadamard(qubit);
}

void QuantumState::applyPhase(std::size_t qubit, double angle) {
    impl_->applyPhase(qubit, angle);
}

void QuantumState::applyCNOT(std::size_t control, std::size_t target) {
    impl_->applyCNOT(control, target);
}

void QuantumState::applySingleQubitGate(std::size_t qubit, const Matrix2cd& gate) {
    impl_->applySingleQubitGate(qubit, gate);
}

void QuantumState::applyMeasurement(std::size_t qubit) {
    impl_->applyMeasurement(qubit);
}

void QuantumState::applyGateOptimized(const MatrixXcd& gate) {
    impl_->applyGateOptimized(gate);
}

void QuantumState::prepareState() {
    impl_->prepareState();
}

const VectorXcd& QuantumState::normalizedVector() const noexcept {
    return impl_->normalizedVector();
}

std::size_t QuantumState::getNumQubits() const noexcept {
    return impl_->getNumQubits();
}

std::size_t QuantumState::size() const noexcept {
    return impl_->size();
}

void QuantumState::normalize() {
    impl_->normalize();
}

bool QuantumState::isValid() const noexcept {
    return impl_->isValid();
}




const Eigen::VectorXcd& QuantumState::getStateVector() const noexcept {
    return impl_->getStateVector();
}

std::vector<bool> QuantumState::getMeasurementOutcomes() const {
    return impl_->getMeasurementOutcomes();
}

std::complex<double> QuantumState::getAmplitude(std::size_t index) const {
    return impl_->getAmplitude(index);
}

void QuantumState::setAmplitude(std::size_t index, const std::complex<double>& value) {
    impl_->setAmplitude(index, value);
}

double QuantumState::getCoherence() const noexcept {
    return impl_->getCoherence();
}

double QuantumState::getEntropy() const noexcept {
    return impl_->getEntropy();
}

const MatrixXcd& QuantumState::entanglementMatrix() const {
    return impl_->entanglementMatrix();
}

MatrixXcd QuantumState::generateEntanglement() const {
    return impl_->generateEntanglement();
}

std::vector<MatrixXcd> QuantumState::createLayers() const {
    return impl_->createLayers();
}

double QuantumState::calculateCoherence() const noexcept {
    return impl_->calculateCoherence();
}

double QuantumState::calculateEntropy() const noexcept {
    return impl_->calculateEntropy();
}

void QuantumState::encode(std::vector<double> features) {
    impl_->encode(std::move(features));
}

const std::vector<double>& QuantumState::getFeatures() const {
    return impl_->getFeatures();
}

void QuantumState::generateEntanglementMatrix() {
    impl_->generateEntanglementMatrix();
}

void QuantumState::validateState() const {
    impl_->validateState();
}

MatrixXcd QuantumState::createSingleQubitGate(
    const MatrixXcd& gate, 
    std::size_t target_qubit) const {
    return impl_->createSingleQubitGate(gate, target_qubit);
}

bool QuantumState::operator==(const QuantumState& other) const noexcept {
    return impl_->state_vector_ == other.impl_->state_vector_;
}

} // namespace quids::quantum
}