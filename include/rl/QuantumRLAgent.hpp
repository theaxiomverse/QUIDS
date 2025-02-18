#ifndef QUIDS_RL_QUANTUM_RL_AGENT_HPP
#define QUIDS_RL_QUANTUM_RL_AGENT_HPP

//#include "../StdNamespace.hpp"
#include "../quantum/QuantumTypes.hpp"
#include "../quantum/QuantumCircuit.hpp"
#include "../neural/QuantumPolicyNetwork.hpp"
#include "../neural/QuantumValueNetwork.hpp"

namespace quids::rl {

struct Experience {
    ::std::vector<double> state;
    ::std::vector<double> action;
    double reward;
    ::std::vector<double> next_state;
    bool done;
};

class QuantumRLAgent {
public:
    explicit QuantumRLAgent(::std::size_t state_dim, ::std::size_t action_dim);
    ~QuantumRLAgent();

    ::std::vector<double> selectAction(const ::std::vector<double>& state);
    void update(const Experience& experience);
    void train(::std::size_t num_episodes);
    void save(const ::std::string& path) const;
    void load(const ::std::string& path);

private:
    struct Impl;
    ::std::unique_ptr<Impl> impl_;
    ::std::vector<Experience> replay_buffer_;
    ::std::unique_ptr<neural::QuantumPolicyNetwork> policy_network_;
    ::std::unique_ptr<neural::QuantumValueNetwork> value_network_;
    ::std::unique_ptr<quantum::QuantumCircuit> circuit_;

    void updateMetrics();
    void optimizeQuantumCircuit();
    double computeQuantumAdvantage() const;
};

} // namespace quids::rl

#endif // QUIDS_RL_QUANTUM_RL_AGENT_HPP 