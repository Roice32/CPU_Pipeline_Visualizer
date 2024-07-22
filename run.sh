#!/bin/bash

cd ./build

echo "    > Removing old executable"
rm cpp_processor_model

echo "    > Generating build files with CMake"
cmake build . -DCMAKE_BUILD_TYPE=Debug

echo "    > Building project executable with Makefile"
make

echo "    > Running Program"
if [ $# == 2 ]
then
    ./cpp_processor_model $1 $2
elif [ $# == 1 ]
then
    ./cpp_processor_model $1 ""
else
    echo "Usage: run.sh <.hex source code file> <output file>"
fi

cd ../