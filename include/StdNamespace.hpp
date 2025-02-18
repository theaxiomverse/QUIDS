#ifndef QUIDS_STD_NAMESPACE_HPP
#define QUIDS_STD_NAMESPACE_HPP

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <atomic>
#include <chrono>
#include <array>
#include <optional>

namespace quids {

// Import STL types into quids namespace
using ::std::vector;
using ::std::map;
using ::std::string;
using ::std::unique_ptr;
using ::std::shared_ptr;
using ::std::atomic;
using ::std::array;
using ::std::optional;
using ::std::chrono::system_clock;

} // namespace quids

#endif // QUIDS_STD_NAMESPACE_HPP