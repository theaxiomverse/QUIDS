#ifndef QUANTUM_GATES_HPP
#define QUANTUM_GATES_HPP

#include <Eigen/Dense>
#include <complex>
#include <cmath>
#include "quantum/QuantumTypes.hpp"

namespace quids::quantum {

using Complex = std::complex<double>;
using GateMatrix = Eigen::MatrixXcd;

namespace gates {
    // Standard quantum gates
    inline GateMatrix hadamard() {
        return H;
    }

    inline GateMatrix phase(double angle) {
        return Phase(angle);
    }

    inline GateMatrix cnot() {
        return CNOT;
    }

    // Helper functions to create parameterized gates
    inline GateMatrix Rx(double theta) {
        const Complex i(0, 1);
        const Complex c = cos(theta/2);
        const Complex s = sin(theta/2);
        GateMatrix matrix(2, 2);
        matrix << c, -i*s,
                 -i*s, c;
        return matrix;
    }

    inline GateMatrix Ry(double theta) {
        const Complex c = cos(theta/2);
        const Complex s = sin(theta/2);
        GateMatrix matrix(2, 2);
        matrix << c, -s,
                 s, c;
        return matrix;
    }

    inline GateMatrix Rz(double theta) {
        const Complex i(0, 1);
        const Complex phase = exp(-i * theta/2.0);
        GateMatrix matrix(2, 2);
        matrix << phase, 0,
                 0, std::conj(phase);
        return matrix;
    }

    inline GateMatrix Phase(double phi) {
        Complex phase = std::polar(1.0, phi);
        GateMatrix matrix(2, 2);
        matrix << 1.0, 0.0,
                 0.0, phase;
        return matrix;
    }

    // Constant gates
    const GateMatrix H = []() {
        GateMatrix matrix(2, 2);
        double invSqrt2 = 1.0 / std::sqrt(2.0);
        matrix << invSqrt2, invSqrt2,
                 invSqrt2, -invSqrt2;
        return matrix;
    }();

    const GateMatrix X = []() {
        GateMatrix matrix(2, 2);
        matrix << 0.0, 1.0,
                 1.0, 0.0;
        return matrix;
    }();

    const GateMatrix Y = []() {
        GateMatrix matrix(2, 2);
        matrix << 0.0, -Complex(0.0, 1.0),
                 Complex(0.0, 1.0), 0.0;
        return matrix;
    }();

    const GateMatrix Z = []() {
        GateMatrix matrix(2, 2);
        matrix << 1.0, 0.0,
                 0.0, -1.0;
        return matrix;
    }();

    const GateMatrix S = []() {
        GateMatrix matrix(2, 2);
        matrix << 1.0, 0.0,
                 0.0, Complex(0.0, 1.0);
        return matrix;
    }();

    const GateMatrix T = []() {
        GateMatrix matrix(2, 2);
        matrix << 1.0, 0.0,
                 0.0, std::exp(Complex(0.0, M_PI/4.0));
        return matrix;
    }();

    const GateMatrix CNOT = []() {
        GateMatrix matrix(4, 4);
        matrix << 1.0, 0.0, 0.0, 0.0,
                 0.0, 1.0, 0.0, 0.0,
                 0.0, 0.0, 0.0, 1.0,
                 0.0, 0.0, 1.0, 0.0;
        return matrix;
    }();

} // namespace gates
} // namespace quids::quantum

#endif // QUANTUM_GATES_HPP