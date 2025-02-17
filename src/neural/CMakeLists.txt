set(NEURAL_SOURCES
    QuantumPolicyNetwork.cpp
    BaseQuantumNetwork.cpp
)

# Add source files to parent scope
set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/QuantumPolicyNetwork.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BaseQuantumNetwork.cpp
    PARENT_SCOPE
) 