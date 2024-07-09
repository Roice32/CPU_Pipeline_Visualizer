#pragma once

#include "IExecutionStrategy.cpp"

class ExecuteSimpleMathOp: public IExecutionStrategy
{
public:
    ExecuteSimpleMathOp(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    void executeInstruction(Instruction instr);
};