cmake_minimum_required(VERSION 3.1)

project(stlext)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin")
add_subdirectory(tests)
add_subdirectory(benchmarks)
add_subdirectory(examples)

