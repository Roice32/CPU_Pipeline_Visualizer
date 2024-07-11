#pragma once

#include "IExecutionStrategy.cpp"

class ExecComplexMathOp: public IExecutionStrategy
{
public:
    ExecComplexMathOp(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    void executeInstruction(Instruction instr) override;
    void log(Instruction instr, word r0Result = 0, word r1Result = 0, bool newLine = true) override;
    ~ExecComplexMathOp();
};