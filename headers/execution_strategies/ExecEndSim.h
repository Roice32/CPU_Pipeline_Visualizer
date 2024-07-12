#pragma once

#include "IExecutionStrategy.cpp"

class ExecEndSim: public IExecutionStrategy
{
public:
    ExecEndSim(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS, std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
};