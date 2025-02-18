# Base components with no internal dependencies
add_subdirectory(crypto)      # Uses: OpenSSL, BLAKE3
add_subdirectory(storage)     # Uses: RocksDB, ZSTD

# Core components built in correct dependency order
add_subdirectory(quantum)     # Uses: Eigen3
add_subdirectory(blockchain)  # Uses: crypto, OpenSSL
add_subdirectory(neural)      # Uses: quantum, OpenMP
add_subdirectory(zkp)         # Uses: crypto, GMP

# Generate version and feature information
configure_file(
    ${CMAKE_SOURCE_DIR}/include/version.hpp.in
    ${CMAKE_BINARY_DIR}/include/quids/version.hpp
    @ONLY
)
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/../cmake/config.h.in
    ${CMAKE_BINARY_DIR}/include/quids/config.h 
    @ONLY
)

# Higher level components
add_subdirectory(evm)         # Uses: blockchain, storage
add_subdirectory(rollup)      # Uses: blockchain, zkp, storage, neural

# CLI component
add_library(quids_cli SHARED
    cli/QuidsCommand.cpp
    cli/QuidsCLI.cpp
    cli/commands/StartCommand.cpp
    cli/commands/StopCommand.cpp
    cli/commands/StatusCommand.cpp
)

target_include_directories(quids_cli
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(quids_cli
    PRIVATE
    fmt::fmt
    spdlog::spdlog)

# Control component
add_library(quids_control SHARED
    control/QuidsControl.cpp
)

target_include_directories(quids_control
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(quids_control
    PRIVATE
    blockchain
    evm
    rollup
    fmt::fmt
    spdlog::spdlog)

# Main executable
add_executable(quids_main
    main.cpp
)

target_include_directories(quids_main
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(quids_main
    PRIVATE
    quids_cli
    quids_control)

# Main shared library
add_library(quids SHARED)

# Add sources
target_sources(quids PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(quids
    PUBLIC
    blockchain
    crypto
    quantum
    neural
    zkp
    storage
    rollup
    evm
    quids_cli
    quids_control
    fmt::fmt
    spdlog::spdlog
    PRIVATE
    OpenMP::OpenMP_CXX
    OpenSSL::Crypto
    OpenSSL::SSL
    CURL::libcurl
    ${ZSTD_LIBRARY}
    ${BLAKE3_LIBRARY}
    ${ROCKSDB_LIBRARY}
    ${GMP_LIBRARY}
    Eigen3::Eigen
    ZLIB::ZLIB
)

target_include_directories(quids
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${ROCKSDB_INCLUDE_DIR}
    ${EIGEN3_INCLUDE_DIR}
    ${GMP_INCLUDE_DIR}
)