
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was quids-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)

# Find dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

find_dependency(OpenMP)
find_dependency(OpenSSL 3.0)
find_dependency(fmt 8.0)
find_dependency(ZLIB 1.2)
find_dependency(CURL 7.80)
find_dependency(spdlog 1.9)
find_dependency(Eigen3 3.3)
find_dependency(BLAKE3)
find_dependency(RocksDB)
find_dependency(GMP)
find_dependency(ZSTD)

# Optional dependencies 
find_dependency(MiniUPnPc)
find_dependency(NATPMP)

# Include targets
include("${CMAKE_CURRENT_LIST_DIR}/quids-targets.cmake")
