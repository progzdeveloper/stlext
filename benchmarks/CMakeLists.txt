cmake_minimum_required(VERSION 3.1)

project(stlext-benchmarks)
find_library(PTHREAD_LIBRARY pthread)
find_library(GBENCHMARK_LIBRARY benchmark)
find_library(GBENCHMARK_MAINLIB benchmark_main)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -Wall -Wextra")
set(CMAKE_LFLAGS "${CMAKE_LFLAGS} -fsanitize=address")
include_directories(../ ../catch ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(${PROJECT_NAME}
  bm_bitvector.cpp
  bm_counting_sort.cpp
  bm_main.cpp
  bm_packed_hashtbl.cpp
  bm_stringset.cpp
)
target_link_libraries(${PROJECT_NAME} ${GBENCHMARK_LIBRARY} ${GBENCHMARK_MAINLIB} ${PTHREAD_LIBRARY})

