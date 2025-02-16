#ifndef QUANTUM_STATE_HPP
#define QUANTUM_STATE_HPP

#pragma once

#include "QuantumForward.hpp"
#include "QuantumTypes.hpp"
#include <Eigen/Dense>
#include <vector>
#include <complex>
#include <memory>
#include <unordered_map>
#include <array>
#include <cstdint>

namespace quids {
namespace quantum {

class QuantumState {
public:
    // Default constructor
    QuantumState();

    // Forward declare Impl
    class Impl;

    // Constructor for n-qubit state initialized to |0...0>
    explicit QuantumState(size_t num_qubits);

    // Constructor from state vector
    explicit QuantumState(const Eigen::VectorXcd& state_vector);

    // Copy constructor
    QuantumState(const QuantumState& other);

    // Move constructor
    QuantumState(QuantumState&& other) noexcept;

    // Copy assignment
    QuantumState& operator=(const QuantumState& other);

    // Move assignment
    QuantumState& operator=(QuantumState&& other) noexcept;

    // Destructor
    ~QuantumState();

    // Core quantum operations
    size_t getNumQubits() const;
    size_t size() const;
    void normalize();
    bool isValid() const;

    // State access
    const Eigen::VectorXcd& getStateVector() const;
    std::vector<bool> getMeasurementOutcomes() const;
    std::complex<double> getAmplitude(size_t index) const;
    void setAmplitude(size_t index, const std::complex<double>& value);

    // Gate operations
    void applyHadamard(size_t qubit);
    void applyPhase(size_t qubit, double angle);
    void applyCNOT(size_t control, size_t target);
    void applyMeasurement(size_t qubit);
    void applySingleQubitGate(size_t qubit, const Eigen::Matrix2cd& gate);
    void applyGateOptimized(const Eigen::MatrixXcd& gate);

    // Quantum metrics
    double getCoherence() const;
    double getEntropy() const;
    const Eigen::VectorXcd& normalizedVector() const;
    const Eigen::MatrixXcd& entanglementMatrix() const;

    // Additional methods
    void generateEntanglement() const;
    std::vector<Eigen::MatrixXcd> createLayers() const;
    double calculateCoherence() const;
    double calculateEntropy() const;
    void prepareState();

    // Comparison operator
    bool operator==(const QuantumState& other) const;

private:
    void generateEntanglementMatrix();
    void validateState() const;
    Eigen::MatrixXcd createSingleQubitGate(const Eigen::MatrixXcd& gate, size_t target_qubit) const;

    std::unique_ptr<Impl> impl_;
    static std::unordered_map<size_t, QuantumState> state_cache_;
};

} // namespace quantum
} // namespace quids

#endif // QUANTUM_STATE_HPP