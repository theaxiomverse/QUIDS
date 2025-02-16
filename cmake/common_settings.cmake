# Find required packages
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_C_FLAGS}")
endif()

find_package(GTest REQUIRED)
find_package(OpenSSL 3.0 REQUIRED)
find_package(fmt REQUIRED)
find_package(ZLIB REQUIRED)
find_package(CURL REQUIRED)
find_package(BLAKE3 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(spdlog REQUIRED)

include_directories()
include_directories(${OPENSSL_INCLUDE_DIR})

# Add binary dir to include path for generated header
include_directories(${CMAKE_BINARY_DIR}/include)

# Set C++ flags for OpenSSL
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${OPENSSL_INCLUDE_DIR}")

# Add OpenSSL to compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Detect the operating system
if(APPLE)
    # macOS specific settings
    include(cmake/macos_settings.cmake)
elseif(WIN32)
    # Windows specific settings
    include(cmake/windows_settings.cmake)
else()
    # Ubuntu specific settings
    include(cmake/ubuntu_settings.cmake)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
