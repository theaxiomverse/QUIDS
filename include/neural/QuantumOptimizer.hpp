#ifndef QUIDS_NEURAL_QUANTUMOPTIMIZER_HPP
#define QUIDS_NEURAL_QUANTUMOPTIMIZER_HPP

#include "BaseQuantumNetwork.hpp"
#include <vector>
#include <memory>

namespace quids::neural {

class QuantumOptimizer {
public:
    // Constructor to initialize the optimizer with a given learning rate.
    explicit QuantumOptimizer(double learningRate);

    // Adjusts the learning rate dynamically if required.
    void setLearningRate(double rate);
    double getLearningRate() const;

    // Applies calculated gradients to a quantum network.
    // Gradients are passed as a vector of values.
    void applyQuantumGradients(BaseQuantumNetwork* network, const std::vector<double>& gradients);

private:
    double learningRate_; // Learning rate for the optimizer.
};

} // namespace quids::neural

#endif // QUIDS_NEURAL_QUANTUMOPTIMIZER_HPP