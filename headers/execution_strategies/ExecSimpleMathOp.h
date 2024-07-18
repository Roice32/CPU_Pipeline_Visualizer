#pragma once

#include "IExecutionStrategy.cpp"

class ExecSimpleMathOp: public IExecutionStrategy
{
private:
    void log(LoggablePackage toLog) override;

public:
    ExecSimpleMathOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        IClockBoundModule* refToEX,
        std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
};