# Rollup component
add_library(rollup STATIC
    AIRollupAgent.cpp
    BatchProcessor.cpp
    CrossRollupBridge.cpp
    DataCompressor.cpp
    EmergencyExit.cpp
    EnhancedRollupMLModel.cpp
    FraudProof.cpp
    L1Bridge.cpp
    MEVProtection.cpp
    OptimisticAdapter.cpp
    ParallelProcessor.cpp
    ProofAggregator.cpp
    RollupBenchmark.cpp
    RollupMLModel.cpp
    RollupStateTransition.cpp
    RollupTransactionAPI.cpp
    StateManager.cpp
    StateTransitionProof.cpp
)

target_link_libraries(rollup
    PRIVATE
    blockchain
    zkp
    storage
    neural # Required for ML models
)

target_include_directories(rollup
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})