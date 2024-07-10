#pragma once

#include "IExecutionStrategy.cpp"

class ExecComplexMathOp: public IExecutionStrategy
{
public:
    ExecComplexMathOp(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    void executeInstruction(Instruction instr);
    ~ExecComplexMathOp();
};