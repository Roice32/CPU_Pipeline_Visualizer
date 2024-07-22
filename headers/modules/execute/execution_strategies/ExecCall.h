#pragma once

#include "IExecutionStrategy.cpp"

class ExecCall: public IExecutionStrategy
{
private:
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> fromDEtoMe;

    std::string log(LoggablePackage toLog) override;

public:
    ExecCall(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
        IClockBoundModule* refToEX,
        std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
};