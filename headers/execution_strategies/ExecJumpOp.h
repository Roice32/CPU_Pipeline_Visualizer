#pragma once

#include "IExecutionStrategy.cpp"

class ExecJumpOp: public IExecutionStrategy
{
public:
    ExecJumpOp(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, std::vector<word>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
};