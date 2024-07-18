#pragma once

#include "IExecutionStrategy.cpp"

class ExecCmp: public IExecutionStrategy
{
private:
    void log(LoggablePackage toLog) override;

public:
    ExecCmp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
};