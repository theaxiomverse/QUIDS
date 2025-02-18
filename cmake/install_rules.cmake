# Installation configuration

include(GNUInstallDirs)

# Install all targets together to ensure proper export set handling
install(TARGETS 
    project_includes
    crypto
    storage
    quantum
    blockchain
    neural
    zkp
    evm
    rollup
    quids_cli
    quids_control
    quids_main
    quids
    EXPORT quids-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Install generated headers
install(FILES
    ${CMAKE_BINARY_DIR}/include/quids/version.hpp
    ${CMAKE_BINARY_DIR}/include/quids/config.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/quids
)

# Export targets
install(EXPORT quids-targets
    FILE quids-targets.cmake
    NAMESPACE quids::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/quids
)

# Package configuration
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/quids-config.cmake.in
    ${CMAKE_BINARY_DIR}/quids-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/quids
)
write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/quids-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)