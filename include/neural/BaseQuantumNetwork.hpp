//
// Created by nick on 2025-02-11.
//

#ifndef QUIDS_NEURAL_BASEQUANTUMNETWORK_HPP
#define QUIDS_NEURAL_BASEQUANTUMNETWORK_HPP

#include <vector>

namespace quids::neural {
    class BaseQuantumNetwork {
    public:
        virtual ~BaseQuantumNetwork() = default;
        
        // Parameter access
        virtual double getParameter(size_t index) const = 0;
        virtual void setParameter(size_t index, double value) = 0;
        virtual size_t getNumParameters() const = 0;
        
        // Network operations
        virtual void forward() = 0;
        virtual void backward() = 0;
        virtual std::vector<double> getGradients() const = 0;
    };
}

#endif //QUIDS_NEURAL_BASEQUANTUMNETWORK_HPP
