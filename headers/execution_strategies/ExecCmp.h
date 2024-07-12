#pragma once

#include "IExecutionStrategy.cpp"

class ExecCmp: public IExecutionStrategy
{
public:
    ExecCmp(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS, std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
    void log(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true) override;
};