#pragma once

#include "IExecutionStrategy.cpp"

class ExecSimpleMathOp: public IExecutionStrategy
{
public:
    ExecSimpleMathOp(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    void executeInstruction(Instruction instr);
    ~ExecSimpleMathOp();
};