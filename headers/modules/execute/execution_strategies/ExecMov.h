#pragma once

#include "IExecutionStrategy.cpp"

class ExecMov: public IExecutionStrategy
{
private:
    std::string log(LoggablePackage toLog) override;

public:
    ExecMov(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        IClockBoundModule* refToEX,
        std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
};