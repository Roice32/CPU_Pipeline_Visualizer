#pragma once

#include "IExecutionStrategy.cpp"

class ExecCmp: public IExecutionStrategy
{
public:
    ExecCmp(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    void executeInstruction(Instruction instr);
};