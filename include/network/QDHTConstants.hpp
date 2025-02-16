#pragma once
#include <cstddef>

namespace quids::network {

// Constants for QDHT
constexpr size_t QDHT_ID_LENGTH = 256;  // Length of node IDs in bits (quantum-secure)
constexpr size_t QDHT_K = 20;           // Size of k-buckets
constexpr size_t QDHT_ALPHA = 3;        // Number of parallel lookups
constexpr size_t QDHT_B = 256;          // Number of bits per digit

} // namespace quids::network 