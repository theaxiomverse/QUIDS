#include "quantum/QuantumCircuitConfig.hpp"  // Must come first for complete type
#include "quantum/QuantumCircuit.hpp"
#include "quantum/QuantumGates.hpp"
#include "quantum/QuantumOperations.hpp"

#include <random>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <limits>
#include <ostream>
#include <memory>
#include <algorithm>

namespace quids::quantum {  // Using nested namespace declaration

// Type aliases in an internal namespace to avoid pollution
    namespace types {
        using GateType = ::quids::quantum::GateType;
        using GateMatrix = ::quids::quantum::GateMatrix;
        using QuantumState = ::quids::quantum::QuantumState;
        using QuantumMeasurement = ::quids::quantum::QuantumMeasurement;
        using StateVector = ::quids::quantum::StateVector;
        using QuantumCircuitConfig = ::quids::quantum::QuantumCircuitConfig;
        using ErrorSyndrome = ::quids::quantum::ErrorSyndrome;
    }

    using namespace types;  // Bring types into quantum namespace

    class QuantumCircuit::Impl {
    public:
        // Constants
        static constexpr std::size_t MAX_QUBITS = 63;  // Maximum number of qubits for 64-bit system

        // Constructor with member initializer list
        explicit Impl(std::size_t numQubits)
                : state_(StateVector::Zero(calculateStateSize(numQubits))),
                  config_{validateNumQubits(numQubits)},
                  customGates_{} {
            initializeState();
            initializeGateCache();
        }

        // Static methods
        static constexpr std::size_t calculateStateSize(std::size_t numQubits) noexcept {
            return 1ull << numQubits;
        }

        // Public interface methods
        void resetState() noexcept {
            initializeState();
        }

        // State management methods with strong exception guarantees
        void loadState(const StateVector& state) {
            validateStateSize(state.size());
            state_ = state;
        }

        [[maybe_unused]] void loadState(QuantumState&& state) {
            validateStateSize(state.size());
            state_ = static_cast<const StateVector&>(state);
        }

        [[nodiscard]] QuantumState getState() const {
            return QuantumState(state_);
        }

        [[nodiscard]] std::size_t getNumQubits() const noexcept {
            return config_.numQubits;
        }

    private:
        // Private helper methods
        static std::size_t validateNumQubits(std::size_t numQubits) {
            if (numQubits == 0 || numQubits > MAX_QUBITS) {
                throw std::invalid_argument(
                        "Number of qubits must be between 1 and " +
                        std::to_string(MAX_QUBITS)
                );
            }
            return numQubits;
        }

        void validateStateSize(std::size_t stateSize) const {
            if (calculateStateSize(stateSize) != calculateStateSize(config_.numQubits)) {
                throw std::invalid_argument(
                        "State dimension mismatch. Expected: " +
                        std::to_string(calculateStateSize(config_.numQubits)) +
                        ", Got: " + std::to_string(stateSize)
                );
            }
        }

        void initializeState() noexcept {
            try {
                state_.setZero();
                state_(0) = std::complex<double>(1.0, 0.0);  // Set to |0...0⟩ state
            } catch (...) {
                // Handle any unexpected exceptions in release builds
                std::terminate();
            }
        }

        void initializeGateCache() {
            // TODO: Implement gate cache initialization
            gateCache_.reserve(INITIAL_CACHE_SIZE);
        }

        // Member variables in initialization order
        StateVector state_;
        QuantumCircuitConfig config_;
        std::unordered_map<std::string, OperatorMatrix> customGates_;
        std::unordered_map<GateType, GateMatrix> gateCache_;
        static constexpr std::size_t INITIAL_CACHE_SIZE = 16;
    };

// QuantumCircuit implementation
    QuantumCircuit::QuantumCircuit(std::size_t numQubits)
            : impl_(std::make_unique<Impl>(numQubits)) {
        if (!impl_) {
            throw std::runtime_error("Failed to initialize quantum circuit");
        }
    }

    QuantumCircuit::~QuantumCircuit() = default;



   QuantumState QuantumCircuit::execute(const QuantumState& state) const {
        if (!impl_) {
            throw std::runtime_error("Circuit not properly initialized");
        }
        impl_->loadState(state);
    }



}  // namespace quids::quantum
