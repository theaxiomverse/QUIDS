#!/bin/bash

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "CMake not found, installing..."
    # Download and install CMake
    curl -LO https://cmake.org/files/v3.31/cmake-3.31.5-windows-x86_64.msi
    msiexec /i cmake-3.31.5-windows-x86_64.msi /quiet
    rm cmake-3.31.5-windows-x86_64.msi
    # Add CMake to PATH
    export PATH="/c/Program Files/CMake/bin:$PATH"
fi

# Remove the build directory if it exists
if [ -d "build" ]; then
    rm -rf build
fi

# Create build directory
mkdir -p build
cd build

# Run CMake and build
cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Release

echo "Build for Windows completed!"
