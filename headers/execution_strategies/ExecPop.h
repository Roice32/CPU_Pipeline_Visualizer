#pragma once

#include "IExecutionStrategy.cpp"

class ExecPop: public IExecutionStrategy
{
public:
    ExecPop(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
};