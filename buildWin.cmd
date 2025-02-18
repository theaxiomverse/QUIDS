@echo off

:: Create build directory
if not exist build mkdir build
cd build

:: Configure with CMake
cmake -G "Visual Studio 17 2022" -A x64 ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DBUILD_SHARED_LIBS=ON ^
      -DBUILD_TESTS=ON ^
      ..

:: Build
cmake --build . --config Release

:: Run tests
ctest -C Release --output-on-failure