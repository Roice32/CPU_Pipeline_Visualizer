#!/bin/bash

cd ./build

echo "    > Removing old executable"
rm cpp_processor_model

echo "    > Generating build files with CMake"
cmake build . -DCMAKE_BUILD_TYPE=Debug

echo "    > Building project executable with Makefile"
make

cd ../tests/inputs
echo "    > Running Tests"
for testFile in *;
do
    if [ "$testFile" == "*.asm" ];
    then
        cd ../../utils
        python3 ./pyasm.py --infile ../tests/inputs/$testFile --outfile ../tests/inputs/$testFile.hex
        ../build/cpp_processor_model ../tests/inputs/$testFile.hex ../tests/outputs/$testFile.txt
        cd ../tests/inputs
    fi
done

cd ../