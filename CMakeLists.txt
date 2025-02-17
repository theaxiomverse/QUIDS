cmake_minimum_required(VERSION 3.20)
project(QUIDS VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set macOS specific options
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "Minimum macOS deployment version")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# Find dependencies
find_package(OpenMP REQUIRED)
find_package(Eigen3 3.4 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -fPIC")
endif()

# Define source files
set(SOURCES
    src/blockchain/AIBlock.cpp
    src/blockchain/StandardTransaction.cpp
    src/neural/QuantumPolicyNetwork.cpp
    src/neural/BaseQuantumNetwork.cpp
)

# Create library target
add_library(quids STATIC ${SOURCES})

# Set include directories
target_include_directories(quids
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${OPENSSL_INCLUDE_DIR}
        ${CURL_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIR}
)

# Link dependencies
target_link_libraries(quids
    PUBLIC
        OpenMP::OpenMP_CXX
        Eigen3::Eigen
        OpenSSL::SSL
        OpenSSL::Crypto
        CURL::libcurl
        ZLIB::ZLIB
        fmt::fmt
        spdlog::spdlog
)

# Install rules
install(TARGETS quids
    EXPORT quids-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Export targets
install(EXPORT quids-targets
    FILE quids-targets.cmake
    NAMESPACE quids::
    DESTINATION lib/cmake/quids
)
