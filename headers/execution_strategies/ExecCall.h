#pragma once

#include "ExecPush.h"
#include "IExecutionStrategy.cpp"

class ExecCall: public IExecutionStrategy
{
private:
    ExecPush* pushHelper;

public:
    ExecCall(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers, ExecPush* helper);
    void executeInstruction(Instruction instr) override;
    void log(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true) override;
    ~ExecCall();
};