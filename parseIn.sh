#!/bin/bash

cd ./utils

if [ $# != 1 ]
then
    echo "Usage: parseIn.sh <.asm input file name>"
else
    python3 ./pyasm.py --infile ../program_files/$1.asm --outfile ../program_files/$1.hex
fi

cd ../