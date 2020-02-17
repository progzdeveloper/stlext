#!/bin/bash

echo "Creating project root..."
./boostrap.cmake.sh > CMakeLists.txt
echo "done."

echo "Creating stlext project..."
cd stlext
./bootstrap.cmake.stlext.sh > CMakeLists.txt
cd ..
echo "done."


echo "Creating tests project..."
cd tests
./bootstrap.cmake.tests.sh > CMakeLists.txt
cd ..
echo "done."


echo "Creating benchmarks project..."
cd benchmarks
./bootstrap.cmake.gbenchmarks.sh > CMakeLists.txt
cd ..
echo "done."


echo "Creating examples project..."
cd examples
./bootstrap.cmake.examples.sh > CMakeLists.txt
cd ..
echo "done."

#echo "Creating build directory..."
#mkdir build
#cd build
#cmake ../
#make

