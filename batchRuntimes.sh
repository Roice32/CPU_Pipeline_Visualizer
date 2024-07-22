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
    for index in $(seq 1 $2);
    do
        ./cpp_processor_model $1 ../program_files/batch"$index".txt
        echo "Run #$index:"
        tail -1 ../program_files/batch"$index".txt
    done
else
    echo "Usage: run.sh <.hex source code file> <number of times to run>"
fi

cd ../