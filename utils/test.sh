#!/bin/bash

cd ./build

echo "    > Removing old executable"
rm cpp_processor_model

echo "    > Generating build files with CMake"
cmake build . -DCMAKE_BUILD_TYPE=Debug

echo "    > Building project executable with Makefile"
make

echo "    > Running test"
if [ $# == 1 ]
then
    ./cpp_processor_model ../tests/inputs/$1.hex ../tests/outputs/$1.txt ../tests/mem_dump.txt
else
    echo "Usage: test.sh <name of input file>"
fi

cd ../