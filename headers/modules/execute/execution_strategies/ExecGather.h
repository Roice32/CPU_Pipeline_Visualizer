#pragma once

#include "IExecutionStrategy.cpp"

class ExecGather: public IExecutionStrategy
{
public:
    ExecGather(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
        IClockBoundModule* refToEX,
        std::shared_ptr<CPURegisters> registers);
    void executeInstruction(SynchronizedDataPackage<Instruction> instrPackage) override;
};