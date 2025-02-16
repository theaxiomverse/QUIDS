#ifndef NEURAL_BASEQUANTUMNETWORK_HPP
#define NEURAL_BASEQUANTUMNETWORK_HPP

#include <vector>
#include <string>
#include <memory>
#include "neural/BaseQuantumNetwork.hpp"

namespace neural {

class BaseQuantumNetwork {
public:
    // Virtual destructor to allow proper cleanup of derived classes
    virtual ~BaseQuantumNetwork() = default;

    // Pure virtual method to calculate gradients; must be implemented in derived classes
    virtual std::vector<double> calculateQuantumGradients() const = 0;

    // Get specific parameter by index
    virtual double getParameter(size_t index) const = 0;

    // Set specific parameter by index
    virtual void setParameter(size_t index, double value) = 0;

    // Get all parameters of the network as a vector
    virtual std::vector<double> getQuantumParameters() const = 0;

    // Load network state from a file
    virtual void loadNetworkState(const std::string& filePath) = 0;

    // Save network state to a file
    virtual void saveNetworkState(const std::string& filePath) const = 0;
};

} // namespace neural

#endif // NEURAL_BASEQUANTUMNETWORK_HPP