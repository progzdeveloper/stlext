#!/bin/bash

echo 'cmake_minimum_required(VERSION 3.1)'
echo ""
echo 'project(stlext-tests)'
echo 'find_library(PTHREAD_LIBRARY pthread)'
echo 'set(CMAKE_CXX_STANDARD 14)'
echo 'set(CMAKE_CXX_STANDARD_REQUIRED True)'
echo 'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -Wall -Wextra")'
echo 'set(CMAKE_LFLAGS "${CMAKE_LFLAGS} -fsanitize=address")'
echo 'include_directories(../ ../catch ${CMAKE_CURRENT_SOURCE_DIR})'
echo 'add_executable(${PROJECT_NAME}'
echo '  algorithm/experimental.cpp'
echo '  algorithm/sorting.cpp' 
echo '  bitvector/bitvector.cpp'
echo '  compability/c++11_algo.cpp'
echo '  compability/c++14_algo.cpp'
echo '  compability/c++17_algo.cpp'
echo '  compability/c++20_algo.cpp' 
echo '  compact/string.cpp'
echo '  compact/vector.cpp'
echo '  compact/wstring.cpp'
echo '  components/class_factory.cpp'
echo '  containers/packed_hashtbl.cpp' 
echo '  containers/stringset.cpp'
echo '  functional/predicates.cpp' 
echo '  iostreams/base16.cpp'
echo '  iostreams/base64.cpp'
echo '  iostreams/binrep.cpp'
echo '  iostreams/getline.cpp' 
echo '  iostreams/ostream_joiner.cpp'
echo '  iostreams/ratio.cpp'
echo '  iterator/iterators.cpp' 
echo '  containers/lru_cache.cpp' 
echo '  main.cpp'
echo ')'
echo 'target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIBRARY})'
#echo 'install(TARGETS ${PROJECT_NAME} DESTINATION ../bin)'
echo ""

