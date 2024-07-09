#pragma once

#include "IExecutionStrategy.cpp"

class ExecMov: public IExecutionStrategy
{
public:
    ExecMov(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    void executeInstruction(Instruction instr);
};