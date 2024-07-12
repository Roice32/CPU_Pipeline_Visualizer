#!/bin/bash

cd ./build

echo "    > Removing old executable"
rm cpp_processor_model

echo "    > Generating build files with CMake"
cmake build .

echo "    > Building project executable with Makefile"
make

echo "    > Running Program"
if [ $# != 1 ]
then
    echo "Usage: run.sh <.hex source code file>"
else
    ./cpp_processor_model $1
fi

cd ../