#!/bin/bash

cd ./build

echo "    > Removing old executable"
rm CPU_Pipeline_Visualizer

echo "    > Generating build files with CMake"
cmake build . -DCMAKE_BUILD_TYPE=Debug

echo "    > Building project executable with Makefile"
make

echo "    > Running test"
if [ $# == 1 ]
then
    ./CPU_Pipeline_Visualizer $1.hex $1.log ../tests/mem_dump.log
else
    echo "Usage: test.sh <name of input file>"
fi

cd ../