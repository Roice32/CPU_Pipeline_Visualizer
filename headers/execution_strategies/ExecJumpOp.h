#pragma once

#include "IExecutionStrategy.cpp"

class ExecJumpOp: public IExecutionStrategy
{
public:
    ExecJumpOp(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    void executeInstruction(Instruction instr) override;
    ~ExecJumpOp();
};