#pragma once

#include "ExecPop.h"
#include "IExecutionStrategy.cpp"

class ExecRet: public IExecutionStrategy
{
private:
    ExecPop* popHelper;

public:
    ExecRet(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers, ExecPop* helper);
    void executeInstruction(Instruction instr);
};