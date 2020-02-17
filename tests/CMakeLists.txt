cmake_minimum_required(VERSION 3.1)

project(stlext-tests)
find_library(PTHREAD_LIBRARY pthread)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -Wall -Wextra")
set(CMAKE_LFLAGS "${CMAKE_LFLAGS} -fsanitize=address")
include_directories(../ ../catch)
add_executable(${PROJECT_NAME}
  algorithm/experimental.cpp
  algorithm/sorting.cpp
  bitvector/bitvector.cpp
  compability/c++11_algo.cpp
  compability/c++14_algo.cpp
  compability/c++17_algo.cpp
  compability/c++20_algo.cpp
  compact/string.cpp
  compact/vector.cpp
  compact/wstring.cpp
  components/class_factory.cpp
  containers/packed_hashtbl.cpp
  containers/stringset.cpp
  functional/predicates.cpp
  iostreams/base16.cpp
  iostreams/base64.cpp
  iostreams/binrep.cpp
  iostreams/getline.cpp
  iostreams/ostream_joiner.cpp
  iostreams/ratio.cpp
  iterator/iterators.cpp
  containers/lru_cache.cpp
  main.cpp
)
target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIBRARY})

