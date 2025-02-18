#ifndef QUIDS_QUANTUM_CIRCUIT_HPP
#define QUIDS_QUANTUM_CIRCUIT_HPP

#include "QuantumTypes.hpp"
#include "QuantumState.hpp"
#include "QuantumGates.hpp"
#include "QuantumOperations.hpp"
//#include "StdNamespace.hpp"
#include <vector>
#include <memory>
#include <string>
#include <cstddef>

namespace quids::quantum {

using GateType = ::quids::quantum::GateType;
using QuantumState = ::quids::quantum::QuantumState;
using OperatorMatrix = ::quids::quantum::OperatorMatrix;

/**
 * @brief Represents a quantum circuit for quantum computation
 * 
 * This class implements a quantum circuit that can be constructed from quantum
 * gates and executed on quantum states. It supports various quantum operations,
 * measurements, and circuit optimization techniques.
 */
class QuantumCircuit {
public:
    /// Forward declaration of implementation class
    class Impl;

    /**
     * @brief Creates a quantum circuit with specified number of qubits
     * @param numQubits Number of qubits in the circuit
     * @throws std::invalid_argument if numQubits is 0
     */
    explicit QuantumCircuit(std::size_t numQubits);

    // Delete copy operations
    QuantumCircuit(const QuantumCircuit&) = delete;
    QuantumCircuit& operator=(const QuantumCircuit&) = delete;

    // Enable move operations
    QuantumCircuit(QuantumCircuit&&) noexcept = default;
    QuantumCircuit& operator=(QuantumCircuit&&) noexcept = default;

    /**
     * @brief Virtual destructor
     */
    virtual ~QuantumCircuit();

    /**
     * @brief Adds a single-qubit gate to the circuit
     * @param type Type of quantum gate to add
     * @param qubit Target qubit index
     * @throws std::out_of_range if qubit index is invalid
     * @throws std::invalid_argument if gate type is invalid
     */
    void addGate(GateType type, std::size_t qubit);

    /**
     * @brief Adds a controlled gate to the circuit
     * @param type Type of quantum gate to control
     * @param control Control qubit index
     * @param target Target qubit index
     * @throws std::out_of_range if qubit indices are invalid
     * @throws std::invalid_argument if gate type is invalid
     */
    void addControlledGate(GateType type, std::size_t control, std::size_t target);

    /**
     * @brief Adds a custom gate to the circuit
     * @param gate Custom unitary matrix
     * @param qubit Target qubit index
     * @throws std::out_of_range if qubit index is invalid
     * @throws std::invalid_argument if gate matrix is not unitary
     */
    void addCustomGate(const OperatorMatrix& gate, std::size_t qubit);

    /**
     * @brief Adds a measurement operation to the circuit
     * @param qubit Qubit to measure
     * @throws std::out_of_range if qubit index is invalid
     */
    void addMeasurement(std::size_t qubit);
    
    /**
     * @brief Executes the circuit on an initial state
     * @param initialState Starting quantum state
     * @return Final quantum state after circuit execution
     * @throws std::invalid_argument if state dimensions don't match circuit
     */
    [[nodiscard]] QuantumState execute(const QuantumState& initialState) const;

    /**
     * @brief Measures all qubits in the circuit
     * @return Vector of measurement results
     */
    [[nodiscard]] std::vector<bool> measure() const;
    
    /**
     * @brief Gets the depth of the circuit
     * @return Number of layers in the circuit
     */
    [[nodiscard]] std::size_t depth() const noexcept;

    /**
     * @brief Gets the size of the circuit
     * @return Total number of gates in the circuit
     */
    [[nodiscard]] std::size_t size() const noexcept;

    /**
     * @brief Gets the number of qubits in the circuit
     * @return Number of qubits
     */
    [[nodiscard]] std::size_t numQubits() const noexcept;
    
    /**
     * @brief Optimizes the circuit by combining and simplifying gates
     */
    void optimize();

    /**
     * @brief Calculates the computational cost of the circuit
     * @return Cost metric value
     */
    [[nodiscard]] double calculateCost() const noexcept;
    
    /**
     * @brief Validates the circuit structure
     * @return true if circuit is valid, false otherwise
     */
    [[nodiscard]] bool validate() const noexcept;

    /**
     * @brief Gets error messages from validation
     * @return Vector of error messages
     */
    [[nodiscard]] std::vector<std::string> getErrors() const;
    
    /**
     * @brief Converts circuit to string representation
     * @return String describing the circuit
     */
    [[nodiscard]] std::string toString() const;

    /**
     * @brief Clears all gates from the circuit
     */
    void clear() noexcept;

private:
    /// Implementation pointer
    std::unique_ptr<Impl> impl_;
};

} // namespace quids::quantum

#endif // QUIDS_QUANTUM_CIRCUIT_HPP