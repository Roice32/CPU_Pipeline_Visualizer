#pragma once

#include "IExecutionStrategy.cpp"

class ExecJumpOp: public IExecutionStrategy
{
public:
    ExecJumpOp(InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithLS, CPURegisters* registers);
    void executeInstruction(Instruction instr) override;
    ~ExecJumpOp();
};