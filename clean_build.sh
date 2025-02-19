#!/bin/bash

# List of common build files and directories
BUILD_FILES=(
    "CMakeCache.txt"
    "CMakeFiles"
    "Makefile"
    "cmake_install.cmake"
    "*.o"
    "*.obj"
    "*.so"
    "*.dll"
    "*.exe"
    "*.out"
    "*.a"
    "*.lib"
    "*.pdb"
)

# Delete each build file/directory
for file in "${BUILD_FILES[@]}"; do
    find . -name "$file" -exec rm -rf {} +
done

echo "Build files deleted from the root directory."