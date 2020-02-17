#!/bin/bash

echo 'cmake_minimum_required(VERSION 3.1)'
echo ""
echo 'project(stlext)'
echo 'set(CMAKE_CXX_STANDARD 14)'
echo 'set(CMAKE_CXX_STANDARD_REQUIRED True)'
echo 'set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin")'
echo 'add_subdirectory(tests)'
echo 'add_subdirectory(benchmarks)'
echo 'add_subdirectory(examples)'
echo ""
