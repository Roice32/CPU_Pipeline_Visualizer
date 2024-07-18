#pragma once

#include "ClockSyncPackage.h"
#include "IExecutionStrategy.cpp"

class ExecEndSim: public IExecutionStrategy
{
private:
    std::shared_ptr<ClockSyncPackage> clockSyncVars;

public:
    ExecEndSim(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        IClockBoundModule* refToEX,
        std::shared_ptr<CPURegisters> registers,
        std::shared_ptr<ClockSyncPackage> clockSyncVars);
    void executeInstruction(Instruction instr) override;
};