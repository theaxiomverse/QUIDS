# CMake generated Testfile for 
# Source directory: /Volumes/BIGCODE/quids-evm-cpp/tests
# Build directory: /Volumes/BIGCODE/quids-evm-cpp/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(enhanced_ml_tests "/Volumes/BIGCODE/quids-evm-cpp/tests/enhanced_ml_tests")
set_tests_properties(enhanced_ml_tests PROPERTIES  _BACKTRACE_TRIPLES "/Volumes/BIGCODE/quids-evm-cpp/tests/CMakeLists.txt;60;add_test;/Volumes/BIGCODE/quids-evm-cpp/tests/CMakeLists.txt;0;")
add_test(rollup_benchmarks "/Volumes/BIGCODE/quids-evm-cpp/tests/enhanced_ml_tests" "--gtest_filter=\"RollupBenchmarkTest.*\"")
set_tests_properties(rollup_benchmarks PROPERTIES  _BACKTRACE_TRIPLES "/Volumes/BIGCODE/quids-evm-cpp/tests/CMakeLists.txt;66;add_test;/Volumes/BIGCODE/quids-evm-cpp/tests/CMakeLists.txt;0;")
