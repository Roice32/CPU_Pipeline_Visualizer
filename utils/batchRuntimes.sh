#!/bin/bash

cd ./build

echo "    > Removing old executable"
rm CPU_Pipeline_Visualizer

echo "    > Generating build files with CMake"
cmake build . -DCMAKE_BUILD_TYPE=Debug

echo "    > Building project executable with Makefile"
make

echo "    > Running Program"
if [ $# == 2 ]
then
    for index in $(seq 1 $2);
    do
        ./CPU_Pipeline_Visualizer $1 ../tests/batch/"$index".log
        echo "Run #$index:"
        tail -1 ../tests/batch/"$index".log
    done
else
    echo "Usage: run.sh <.hex source code file> <number of times to run>"
fi

cd ../