#!/bin/bash

echo 'cmake_minimum_required(VERSION 3.1)'
echo ""
echo 'project(stlext-benchmarks)'
echo 'find_library(PTHREAD_LIBRARY pthread)'
echo 'find_library(GBENCHMARK_LIBRARY benchmark)'
echo 'find_library(GBENCHMARK_MAINLIB benchmark_main)'
echo 'set(CMAKE_CXX_STANDARD 14)'
echo 'set(CMAKE_CXX_STANDARD_REQUIRED True)'
echo 'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -Wall -Wextra")'
echo 'set(CMAKE_LFLAGS "${CMAKE_LFLAGS} -fsanitize=address")'
echo 'include_directories(../ ../catch ${CMAKE_CURRENT_SOURCE_DIR})'
echo 'add_executable(${PROJECT_NAME}'
echo '  bm_bitvector.cpp'
echo '  bm_counting_sort.cpp'
echo '  bm_main.cpp'
echo '  bm_packed_hashtbl.cpp'
echo '  bm_stringset.cpp'
echo ')'
echo 'target_link_libraries(${PROJECT_NAME} ${GBENCHMARK_LIBRARY} ${GBENCHMARK_MAINLIB} ${PTHREAD_LIBRARY})'
#echo 'install(TARGETS ${PROJECT_NAME} DESTINATION ../bin)'
echo ""
