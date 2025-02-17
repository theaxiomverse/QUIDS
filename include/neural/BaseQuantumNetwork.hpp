//
// Created by nick on 2025-02-11.
//

#pragma once

#include <vector>
#include <string>
#include <cstddef>

namespace quids::neural {

// STL type aliases
using std::vector;
using std::string;
using std::size_t;

class BaseQuantumNetwork {
public:
    virtual ~BaseQuantumNetwork() = default;
    
    // Parameter access
    [[nodiscard]] virtual double getParameter(size_t index) const = 0;
    virtual void setParameter(size_t index, double value) = 0;
    [[nodiscard]] virtual size_t getNumParameters() const = 0;
    [[nodiscard]] virtual std::vector<double> getGradients() const = 0;
    [[nodiscard]] virtual std::vector<double> getQuantumParameters() const = 0;
    
    // Network operations
    virtual void forward() = 0;
    virtual void backward() = 0;
    virtual vector<double> calculateQuantumGradients() const = 0;
    
    // State management
    virtual void loadNetworkState(const string& filePath) = 0;
    virtual void saveNetworkState(const string& filePath) const = 0;

protected:
    BaseQuantumNetwork() = default;
    BaseQuantumNetwork(const BaseQuantumNetwork&) = default;
    BaseQuantumNetwork& operator=(const BaseQuantumNetwork&) = default;
    BaseQuantumNetwork(BaseQuantumNetwork&&) = default;
    BaseQuantumNetwork& operator=(BaseQuantumNetwork&&) = default;
};

} // namespace quids::neural
