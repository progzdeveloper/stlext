#!/bin/bash

echo "Creating project root..."
./bootstrap.qmake.sh > stlext.pro
echo "done."


echo "Creating stlext project..."
cd stlext
./bootstrap.qmake.stlext.sh > stlext.pro
cd ..
echo "done."


echo "Creating tests project..."
cd tests
./bootstrap.qmake.tests.sh > tests.pro
cd ..
echo "done."


echo "Creating benchmarks project..."
cd benchmarks
./bootstrap.qmake.gbenchmarks.sh > benchmarks.pro
cd ..
echo "done."


echo "Creating examples project..."
cd examples
./bootstrap.qmake.examples.sh > examples.pro
cd ..
echo "done."


