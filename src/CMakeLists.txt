# Source files
file(GLOB_RECURSE SOURCES
    "blockchain/*.cpp"
    "crypto/*.cpp"
    "quantum/*.cpp"
    "neural/*.cpp"
    "zkp/*.cpp"
    "storage/*.cpp"
    "rollup/*.cpp"
    "evm/*.cpp"
    "rollup/StateTransitionProof.cpp"
)

# Organize source files by component
set(CLI_SOURCES
    cli/QuidsCommand.cpp
    cli/QuidsCLI.cpp
    cli/commands/StartCommand.cpp
    cli/commands/StopCommand.cpp
    cli/commands/StatusCommand.cpp
)

set(CONTROL_SOURCES
    control/QuidsControl.cpp
)

# Combine all sources
set(QUIDS_SOURCES
    ${CLI_SOURCES}
    ${CONTROL_SOURCES}
    ${SOURCES}
)

# Modify the existing quids_core library
add_library(quids_core SHARED ${QUIDS_SOURCES})

# Add core dependencies
target_link_libraries(quids_core
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
    fmt::fmt
    ${ZLIB_LIBRARIES}
    spdlog::spdlog
)

# Modern CMake way to handle includes
target_include_directories(quids_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${ROCKSDB_INCLUDE_DIR}
        ${JSON_INCLUDE_DIR}
        ${ZLIB_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIR}
        ${VENDOR_DIR}
        ${GMP_INCLUDE_DIR}
        ${libp2p_INCLUDE_DIRS}
)

# Include the main include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Initialize SOURCES variable if not already initialized
if(NOT DEFINED SOURCES)
    set(SOURCES "")
endif()

# Add subdirectories
add_subdirectory(blockchain)
add_subdirectory(neural)

# Add source files to parent scope
set(SOURCES ${SOURCES} PARENT_SCOPE)
