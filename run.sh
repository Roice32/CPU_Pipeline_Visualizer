#!/bin/bash

cd ./build

echo "    > Generating build files with CMake"
cmake build .

echo "    > Building project executable with Makefile"
make

echo "    > Running Program"
./cpp_processor_model

cd ../