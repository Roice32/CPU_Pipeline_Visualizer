#pragma once

#include "ExecPop.h"
#include "IExecutionStrategy.cpp"

class ExecRet: public IExecutionStrategy
{
private:
    std::shared_ptr<ExecPop> popHelper;

    void log(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true) override; 

public:
    ExecRet(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS, std::shared_ptr<CPURegisters> registers, std::shared_ptr<ExecPop> helper);
    void executeInstruction(Instruction instr) override;
};