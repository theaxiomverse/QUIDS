# EVM component
add_library(evm STATIC
    Address.cpp
    Compression.cpp
    EVMExecutor.cpp
    ExternalLink.cpp
    FloatingPoint.cpp
    Memory.cpp
    ProofVerification.cpp
    SolidityParser.cpp
    Stack.cpp
    Storage.cpp
)

target_link_libraries(evm
    PRIVATE
    blockchain
    storage
)

target_include_directories(evm
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})