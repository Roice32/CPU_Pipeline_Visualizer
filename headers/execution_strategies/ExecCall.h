#pragma once

#include "ExecPush.h"
#include "IExecutionStrategy.cpp"

class ExecCall: public IExecutionStrategy
{
private:
    ExecPush* pushHelper;

public:
    ExecCall(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers, ExecPush* helper);
    void executeInstruction(Instruction instr);
};