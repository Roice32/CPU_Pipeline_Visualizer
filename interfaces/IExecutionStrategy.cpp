#pragma once

#include <stdio.h>

#include "Execute.h"
#include "Instruction.h"

class IExecutionStrategy
{
protected:
    Execute* EXModule;

public:
    virtual void executeInstruction(Instruction instr) = 0;
    void log(Instruction instr)
    {
        // Later
    }
};