#pragma once


#include "QuantumCircuitConfig.hpp"

#include "QuantumState.hpp"
#include "QuantumTypes.hpp"



namespace quids {
namespace quantum {      


class QuantumCircuit {
public:
    // Constructors
        QuantumCircuit();  // Constructor declaration
    explicit QuantumCircuit(const QuantumCircuitConfig& config);  // Constructor with config
    ~QuantumCircuit();  // Destructor declaration
 // State management    
    void loadState(const StateVector& state);
    StateVector getState() const;

     // Gate operations
       void validate() const;
       void applyLayer(const std::vector<GateOperation>& layer);  // Ensure GateOperation is recognized
       GateMatrix constructGateMatrix(GateType gate) const;       // Ensure GateMatrix is recognized
       void applyGateToState(const GateMatrix& gate, const std::vector<size_t>& qubits);




    // Calculate state overlap
    double calculateStateOverlap(const StateVector& state1, 
                                  const StateVector& state2) const; // Correctly declared as a const member function

};

    }
    } // namespace quids::quantum