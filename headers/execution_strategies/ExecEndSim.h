#pragma once

#include "IExecutionStrategy.cpp"

class ExecEndSim: public IExecutionStrategy
{
public:
    ExecEndSim(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    void executeInstruction(Instruction instr) override;
    ~ExecEndSim();
};