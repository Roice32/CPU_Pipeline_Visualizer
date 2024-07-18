#pragma once

#include "IExecutionStrategy.cpp"

class ExecPush: public IExecutionStrategy
{
public:
    ExecPush(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        IClockBoundModule* refToEX,
        std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
};