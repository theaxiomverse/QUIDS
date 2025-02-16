#pragma once

#include "blockchain/AIBlock.hpp"
#include "blockchain/Transaction.hpp"
#include "blockchain/Account.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

using std::string;
using std::unordered_map;

namespace quids::blockchain {



class Chain {
public:
    Chain() = default;
    ~Chain() = default;

    // Core chain operations
    bool addBlock(const AIBlock& block);
    bool addTransaction(const Transaction& transaction);

private:
    // Chain state
    AIBlock latestBlock;
    Transaction pendingTransaction;
    unordered_map<string, Account> accounts;

    // Internal methods
    bool validateBlock(const AIBlock& block);
    bool validateTransaction(const Transaction& transaction);
    bool updateAccountStates(const AIBlock& block);
}; 