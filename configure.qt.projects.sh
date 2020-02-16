#!/bin/bash

echo "Creating project root..."
./make.qt.projects.sh > stlext.pro
echo "done."


echo "Creating stlext project..."
cd stlext
./make.qt.stlext.sh > stlext.pro
cd ..
echo "done."


echo "Creating tests project..."
cd tests
./make.qt.gtests.sh > tests.pro
cd ..
echo "done."


echo "Creating benchmarks project..."
cd benchmarks
./make.qt.gbenchmarks.sh > benchmarks.pro
cd ..
echo "done."


echo "Creating examples project..."
cd examples
./make.qt.examples.sh > examples.pro
cd ..
echo "done."


