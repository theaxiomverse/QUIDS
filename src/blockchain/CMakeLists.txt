set(BLOCKCHAIN_SOURCES
    AIBlock.cpp
    StandardTransaction.cpp
)

# Add source files to parent scope
set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/AIBlock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StandardTransaction.cpp
    PARENT_SCOPE
) 