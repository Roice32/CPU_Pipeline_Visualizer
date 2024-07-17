#pragma once

#include "ClockSyncPackage.h"
#include "IExecutionStrategy.cpp"

class ExecEndSim: public IExecutionStrategy
{
private:
    std::shared_ptr<ClockSyncPackage> clockSyncVars;

public:
    ExecEndSim(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, std::vector<word>>> commPipeWithLS,
        std::shared_ptr<CPURegisters> registers,
        std::shared_ptr<ClockSyncPackage> clockSyncVars);
    void executeInstruction(Instruction instr) override;
};