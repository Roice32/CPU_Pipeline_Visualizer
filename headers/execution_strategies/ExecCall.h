#pragma once

#include "IExecutionStrategy.cpp"

class ExecCall: public IExecutionStrategy
{
private:
    void log(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true) override;

public:
    ExecCall(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
};