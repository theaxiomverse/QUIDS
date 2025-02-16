# Find required packages
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_C_FLAGS}")
endif()

# Core dependencies
find_package(GTest REQUIRED)
find_package(OpenSSL 3.0 REQUIRED)
find_package(fmt REQUIRED)
find_package(ZLIB REQUIRED)
find_package(CURL REQUIRED)
find_package(BLAKE3 REQUIRED)
find_package(spdlog REQUIRED)

# Find Eigen3
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
if(TARGET Eigen3::Eigen)
    message(STATUS "Found Eigen3: ${EIGEN3_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "Eigen3 not found")
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    ${OPENSSL_INCLUDE_DIR}
    ${EIGEN3_INCLUDE_DIR}
)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# Add OpenSSL flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${OPENSSL_INCLUDE_DIR}")

# Enable compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# OS-specific settings
if(APPLE)
    include(cmake/macos_settings.cmake)
elseif(WIN32)
    include(cmake/windows_settings.cmake)
else()
    include(cmake/ubuntu_settings.cmake)
endif()

# Add project include directory
include_directories(${CMAKE_SOURCE_DIR}/include)
