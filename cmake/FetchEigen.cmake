include(FetchContent)

message(STATUS "Setting up Eigen...")

# Set version and configure options
set(EIGEN_VERSION "3.4.0")
set(BUILD_TESTING OFF CACHE BOOL "Disable Eigen tests")
set(EIGEN_BUILD_DOC OFF CACHE BOOL "Disable Eigen documentation")
set(EIGEN_BUILD_PKGCONFIG OFF CACHE BOOL "Disable pkg-config")
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(eigen_source_dir "/usr/local/Cellar/eigen/3.4.0_1/include/eigen3")

# Verify headers exist before proceeding
if(NOT EXISTS "${eigen_source_dir}/Eigen/Dense")
    message(FATAL_ERROR "Eigen headers not found at ${eigen_source_dir}/Eigen/Dense after population")
endif()

message(STATUS "Eigen headers found at ${eigen_source_dir}")




    add_library(Eigen3::Eigen INTERFACE)
    target_include_directories(Eigen3::Eigen
        SYSTEM
        INTERFACE
        "${eigen_source_dir}"
    )

    # Add compiler-specific options
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(Eigen3::Eigen
            INTERFACE
            -Wno-deprecated-declarations
            -Wno-ignored-attributes
        )
    endif()


# Set compatibility variables
set(EIGEN3_FOUND TRUE)
set(EIGEN3_VERSION_STRING ${EIGEN_VERSION})
set(EIGEN3_INCLUDE_DIRS "${eigen_source_dir}")

message(STATUS "Eigen setup successful:")
message(STATUS "  Version: ${EIGEN_VERSION}")
message(STATUS "  Include path: ${eigen_source_dir}")
message(STATUS "  Dense header: ${eigen_source_dir}/Eigen/Dense")