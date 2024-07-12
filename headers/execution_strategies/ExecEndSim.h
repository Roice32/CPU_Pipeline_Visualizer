#pragma once

#include "IExecutionStrategy.cpp"

class ExecEndSim: public IExecutionStrategy
{
public:
    ExecEndSim(InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithLS, CPURegisters* registers);
    void executeInstruction(Instruction instr) override;
    ~ExecEndSim();
};