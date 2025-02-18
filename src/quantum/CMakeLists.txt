# Quantum component
add_library(quantum STATIC
    QKD.cpp
    QuantumCircuit.cpp
    QuantumConsensus.cpp
    QuantumCrypto.cpp
    QuantumGates.cpp
    QuantumOperations.cpp
    QuantumState.cpp
    QuantumUtils.cpp
)

# Public link to Eigen and project includes since we expose Eigen types in our public headers
target_link_libraries(quantum
    PUBLIC
    Eigen3::Eigen
    $<BUILD_INTERFACE:project_includes>
    $<INSTALL_INTERFACE:quids::project_includes>
)

# Only private includes for implementation files
target_include_directories(quantum
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)