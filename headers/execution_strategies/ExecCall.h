#pragma once

#include "ExecPush.h"
#include "IExecutionStrategy.cpp"

class ExecCall: public IExecutionStrategy
{
private:
    std::shared_ptr<ExecPush> pushHelper;

public:
    ExecCall(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS, std::shared_ptr<CPURegisters> registers, std::shared_ptr<ExecPush> helper);
    void executeInstruction(Instruction instr) override;
    void log(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true) override;
};