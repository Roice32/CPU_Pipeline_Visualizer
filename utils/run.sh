#!/bin/bash

cd ./build

echo "    > Removing old executable"
rm CPU_Pipeline_Visualizer

echo "    > Generating build files with CMake"
cmake build . -DCMAKE_BUILD_TYPE=Debug

echo "    > Building project executable with Makefile"
make

echo "    > Running Program"
if [ $# == 3 ]
then
    ./CPU_Pipeline_Visualizer $1 $2 $3
elif [ $# == 2 ]
then
    ./CPU_Pipeline_Visualizer $1 $2 ""
elif [ $# == 1 ]
then
    ./CPU_Pipeline_Visualizer $1 "" ""
else
    echo "Usage: run.sh <.hex source code file> [output file] [mem dump file]"
fi

cd ../