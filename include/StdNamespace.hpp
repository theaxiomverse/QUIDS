#ifndef QUIDS_STD_NAMESPACE_HPP
#define QUIDS_STD_NAMESPACE_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <atomic>
#include <memory>
#include <chrono>
#include <optional>
#include <algorithm>
#include <functional>
#include <type_traits>

/**
 * @brief Namespace containing standard library type declarations
 * 
 * This namespace imports commonly used types from the std namespace
 * into the quids namespace to avoid namespace qualification issues
 * and provide a centralized location for std type declarations.
 */
namespace quids {

// Basic types
using std::size_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;

// Containers
using std::string;
using std::vector;
using std::array;

// Smart pointers
using std::unique_ptr;
using std::shared_ptr;
using std::weak_ptr;
using std::make_unique;
using std::make_shared;

// Concurrency
using std::atomic;
using std::memory_order;
using std::memory_order_relaxed;
using std::memory_order_acquire;
using std::memory_order_release;
using std::memory_order_acq_rel;
using std::memory_order_seq_cst;

// Chrono
namespace chrono {
    using std::chrono::system_clock;
    using std::chrono::steady_clock;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    using std::chrono::microseconds;
    using std::chrono::nanoseconds;
    using std::chrono::seconds;
    using std::chrono::minutes;
    using std::chrono::hours;
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::time_point;
}

// Utilities
using std::optional;
using std::nullopt;
using std::reference_wrapper;
using std::ref;
using std::cref;
using std::min;
using std::max;
using std::clamp;
using std::swap;

// Type traits
using std::enable_if;
using std::enable_if_t;
using std::is_same;
using std::is_same_v;
using std::remove_reference;
using std::remove_reference_t;
using std::remove_pointer;
using std::remove_pointer_t;
using std::decay;
using std::decay_t;

// Function objects
using std::function;
using std::bind;
using std::plus;
using std::minus;
using std::multiplies;
using std::divides;
using std::modulus;
using std::negate;

} // namespace quids

#endif // QUIDS_STD_NAMESPACE_HPP 