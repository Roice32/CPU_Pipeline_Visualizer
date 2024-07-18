#pragma once

#include "Instruction.h"

class EXLogPackage
{
public:
    Instruction instr;
    word actualParam1;
    word actualParam2;
    bool newLine;

    EXLogPackage(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true):
        instr(instr), actualParam1(actualParam1), actualParam2(actualParam2), newLine(newLine) {};
};