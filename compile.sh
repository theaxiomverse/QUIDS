#!/bin/bash

# Define the build directory
BUILD_DIR="build"

# Check for build type argument
if [ "$1" == "debug" ]; then
    BUILD_TYPE="Debug"
elif [ "$1" == "release" ]; then
    BUILD_TYPE="Release"
else
    BUILD_TYPE="Release"  # Default to Release if no valid argument is provided
    echo "No valid build type specified. Defaulting to Release."
fi

# Create the build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

# Navigate to the build directory
cd "$BUILD_DIR"

# Run CMake to configure the project
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Check if CMake was successful
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
cmake --build . -- -j$(nproc)  # Use all available cores for faster build

# Check if the build was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Executable created."
else
    echo "Build failed!"
    exit 1
fi
