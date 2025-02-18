# Feature checking utilities

include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)

# Check for concepts support
check_cxx_source_compiles("
    #include <concepts>
    template<typename T>
    concept Numeric = std::integral<T> || std::floating_point<T>;
    int main() { return 0; }
" HAVE_CONCEPTS)

if(NOT HAVE_CONCEPTS)
    message(FATAL_ERROR "Compiler does not support C++20 concepts")
endif()

# Check for coroutines
check_cxx_source_compiles("
    #include <coroutine>
    int main() { return 0; }
" HAVE_COROUTINES)

if(NOT HAVE_COROUTINES)
    message(WARNING "Compiler does not support C++20 coroutines")
endif()

# Check for ranges
check_cxx_source_compiles("
    #include <ranges>
    int main() { 
        auto v = std::views::iota(1, 10);
        return 0;
    }
" HAVE_RANGES)

if(NOT HAVE_RANGES)
    message(WARNING "Compiler does not support C++20 ranges")
endif()

# Create config directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/quids)

# Generate config header
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/config.h.in
    ${CMAKE_BINARY_DIR}/include/quids/config.h
    @ONLY)