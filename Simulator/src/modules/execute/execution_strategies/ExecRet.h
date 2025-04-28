#pragma once

#include "IExecutionStrategy.cpp"

class ExecRet: public IExecutionStrategy
{
private:
    std::string log(LoggablePackage toLog) override; 

public:
    ExecRet(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
        IClockBoundModule* refToEX,
        std::shared_ptr<CPURegisters> registers);
    void executeInstruction(SynchronizedDataPackage<Instruction> instrPackage) override;
};