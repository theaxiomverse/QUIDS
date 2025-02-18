#ifndef QUIDS_QUANTUM_QUANTUM_STATE_HPP
#define QUIDS_QUANTUM_QUANTUM_STATE_HPP

#include "QuantumForward.hpp"
#include "QuantumTypes.hpp"
#include <Eigen/Dense>
#include <array>
#include <complex>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace quids::quantum {

/**
 * @brief Represents a quantum state in the quantum computing system
 * 
 * This class implements a quantum state vector with operations for quantum
 * computation. It supports various quantum gates, measurements, and state
 * manipulations required for quantum algorithms.
 */
using StateVector = Eigen::Matrix<std::complex<double>, -1, 1>;

class QuantumState {
public:
    /// Forward declaration of implementation class
    class Impl;

    // Constructors
    explicit QuantumState(const StateVector& state_vector);
    
    // Conversion operators
    operator const StateVector&() const noexcept;
    operator StateVector&() noexcept;

    /**
     * @brief Default constructor creating a zero-qubit state
     */
    QuantumState();

    /**
     * @brief Creates a quantum state with specified number of qubits
     * @param num_qubits Number of qubits in the state
     */
    explicit QuantumState(std::size_t num_qubits);



    /**
     * @brief Copy constructor
     * @param other State to copy from
     */
    QuantumState(const QuantumState& other);

    /**
     * @brief Move constructor
     * @param other State to move from
     */
    QuantumState(QuantumState&& other) noexcept;

    /**
     * @brief Copy assignment operator
     * @param other State to copy from
     * @return Reference to this state
     */
    QuantumState& operator=(const QuantumState& other);

    /**
     * @brief Move assignment operator
     * @param other State to move from
     * @return Reference to this state
     */
    QuantumState& operator=(QuantumState&& other) noexcept;

    /**
     * @brief Destructor
     */
    ~QuantumState();

    /**
     * @brief Gets the number of qubits in the state
     * @return Number of qubits
     */
    [[nodiscard]] std::size_t getNumQubits() const noexcept;

    /**
     * @brief Gets the dimension of the state vector
     * @return Size of the state vector (2^n for n qubits)
     */
    [[nodiscard]] std::size_t size() const noexcept;

    /**
     * @brief Normalizes the quantum state
     * @throws std::runtime_error if state is zero vector
     */
    void normalize();

    /**
     * @brief Resets the quantum state to initial state
     */
    void reset();

    /**
     * @brief Checks if the state is valid quantum state
     * @return true if state is valid, false otherwise
     */
    [[nodiscard]] bool isValid() const noexcept;

    /**
     * @brief Gets the underlying state vector
     * @return Const reference to the state vector
     */
    [[nodiscard]] const Eigen::VectorXcd& getStateVector() const noexcept;

    /**
     * @brief Gets measurement outcomes for all qubits
     * @return Vector of boolean measurement results
     */
    [[nodiscard]] std::vector<bool> getMeasurementOutcomes() const;

    /**
     * @brief Gets amplitude at specified basis state
     * @param index Index in computational basis
     * @return Complex amplitude
     * @throws std::out_of_range if index is invalid
     */
    [[nodiscard]] std::complex<double> getAmplitude(std::size_t index) const;

    /**
     * @brief Sets amplitude at specified basis state
     * @param index Index in computational basis
     * @param value New complex amplitude
     * @throws std::out_of_range if index is invalid
     */
    void setAmplitude(std::size_t index, const std::complex<double>& value);

    // Gate operations
    /**
     * @brief Applies Hadamard gate to specified qubit
     * @param qubit Target qubit
     * @throws std::out_of_range if qubit is invalid
     */
    void applyHadamard(std::size_t qubit);

    /**
     * @brief Applies phase gate with specified angle
     * @param qubit Target qubit
     * @param angle Phase angle in radians
     * @throws std::out_of_range if qubit is invalid
     */
    void applyPhase(std::size_t qubit, double angle);

    /**
     * @brief Applies CNOT gate between control and target qubits
     * @param control Control qubit
     * @param target Target qubit
     * @throws std::out_of_range if either qubit is invalid
     */
    void applyCNOT(std::size_t control, std::size_t target);

    /**
     * @brief Performs measurement on specified qubit
     * @param qubit Qubit to measure
     * @throws std::out_of_range if qubit is invalid
     */
    void applyMeasurement(std::size_t qubit);

    /**
     * @brief Applies arbitrary single-qubit gate
     * @param qubit Target qubit
     * @param gate 2x2 unitary matrix representing the gate
     * @throws std::out_of_range if qubit is invalid
     * @throws std::invalid_argument if gate matrix is not unitary
     */
    void applySingleQubitGate(std::size_t qubit, const Eigen::Matrix2cd& gate);

    /**
     * @brief Applies optimized gate operation
     * @param gate Gate matrix to apply
     * @throws std::invalid_argument if gate dimensions don't match state
     */
    void applyGateOptimized(const Eigen::MatrixXcd& gate);

    /**
     * @brief Applies rotation gate with specified angle
     * @param angle Rotation angle in radians
     */
    void applyRotation(double angle);

    /**
     * @brief Applies rotation gate to specified qubit
     * @param qubit Target qubit
     * @param angle Rotation angle in radians
     * @throws std::out_of_range if qubit is invalid
     */
    void applyRotation(std::size_t qubit, double angle);

    // Quantum metrics
    /**
     * @brief Gets coherence measure of the state
     * @return Coherence value between 0 and 1
     */
    [[nodiscard]] double getCoherence() const noexcept;

    /**
     * @brief Gets von Neumann entropy of the state
     * @return Entropy value
     */
    [[nodiscard]] double getEntropy() const noexcept;

    /**
     * @brief Gets normalized state vector
     * @return Const reference to normalized state vector
     */
    [[nodiscard]] const Eigen::VectorXcd& normalizedVector() const noexcept;

    /**
     * @brief Gets entanglement matrix of the state
     * @return Density matrix showing entanglement
     */
    [[nodiscard]] const Eigen::MatrixXcd& entanglementMatrix() const;

    /**
     * @brief Generates entanglement between qubits
     */
    [[nodiscard]] Eigen::MatrixXcd generateEntanglement() const;

    /**
     * @brief Creates quantum circuit layers
     * @return Vector of matrices representing circuit layers
     */
    [[nodiscard]] std::vector<Eigen::MatrixXcd> createLayers() const;

    /**
     * @brief Calculates coherence metric
     * @return Coherence value
     */
    [[nodiscard]] double calculateCoherence() const noexcept;

    /**
     * @brief Calculates entropy metric
     * @return Entropy value
     */
    [[nodiscard]] double calculateEntropy() const noexcept;

    /**
     * @brief Prepares state for computation
     */
    void prepareState();

    /**
     * @brief Encodes classical data into quantum state
     * @param features Vector of classical features to encode
     */
    void encode(std::vector<double> features);

    /**
     * @brief Gets the encoded features
     * @return Vector of encoded features
     */
    [[nodiscard]] const std::vector<double>& getFeatures() const;

    /**
     * @brief Equality comparison operator
     * @param other State to compare with
     * @return true if states are equal, false otherwise
     */
    [[nodiscard]] bool operator==(const QuantumState& other) const noexcept;

protected:
    /**
     * @brief Generates entanglement matrix for calculations
     */
    void generateEntanglementMatrix();

    /**
     * @brief Validates state vector
     * @throws std::runtime_error if state is invalid
     */
    void validateState() const;

    /**
     * @brief Creates single-qubit gate matrix
     * @param gate Base 2x2 gate matrix
     * @param target_qubit Target qubit
     * @return Full state space gate matrix
     */
    [[nodiscard]] Eigen::MatrixXcd createSingleQubitGate(
        const Eigen::MatrixXcd& gate, 
        std::size_t target_qubit) const;

private:
    /// Implementation pointer
    std::unique_ptr<Impl> impl_;

    /// Static cache for frequently used states
    static std::unordered_map<std::size_t, QuantumState> state_cache_;
};

} // namespace quids::quantum

#endif // QUIDS_QUANTUM_QUANTUM_STATE_HPP