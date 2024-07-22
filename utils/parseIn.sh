#!/bin/bash

cd ./utils

if [ $# != 1 ]
then
    echo "Usage: parseIn.sh <.asm input file name>"
else
    python3 ./pyasm.py --infile $1.asm --outfile $1.hex
fi

cd ../