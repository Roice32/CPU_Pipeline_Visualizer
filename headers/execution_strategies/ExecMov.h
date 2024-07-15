#pragma once

#include "IExecutionStrategy.cpp"

class ExecMov: public IExecutionStrategy
{
private:
    void log(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true) override;

public:
    ExecMov(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS, std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
};