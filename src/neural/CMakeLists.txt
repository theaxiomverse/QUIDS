# Neural network component
add_library(neural STATIC
    BaseQuantumNetwork.cpp
    QuantumPolicyNetwork.cpp
    QuantumValueNetwork.cpp
)

target_include_directories(neural
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${EIGEN3_INCLUDE_DIR}
)

target_link_libraries(neural
    PUBLIC
        quantum
    PRIVATE
        Eigen3::Eigen
        OpenMP::OpenMP_CXX)