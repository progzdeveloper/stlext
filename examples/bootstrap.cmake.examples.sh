#!/bin/bash

echo 'cmake_minimum_required(VERSION 3.1)'
echo ""
echo 'project(stlext-examples)'
echo 'find_library(PTHREAD_LIBRARY pthread)'
echo 'set(CMAKE_CXX_STANDARD 14)'
echo 'set(CMAKE_CXX_STANDARD_REQUIRED True)'
echo 'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")'
echo 'include_directories(../ ${CMAKE_CURRENT_SOURCE_DIR})'
echo 'add_executable(${PROJECT_NAME} main.cpp)'
echo 'target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIBRARY})'
#echo 'install(TARGETS ${PROJECT_NAME} DESTINATION ../bin)'

