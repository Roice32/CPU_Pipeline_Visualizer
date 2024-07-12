#pragma once

#include "IExecutionStrategy.cpp"

class ExecSimpleMathOp: public IExecutionStrategy
{
public:
    ExecSimpleMathOp(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS, std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
    void log(Instruction instr, word actualParam1 = 0, word result = 0, bool newLine = 0) override;
};