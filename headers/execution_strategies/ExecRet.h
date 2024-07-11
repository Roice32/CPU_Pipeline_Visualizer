#pragma once

#include "ExecPop.h"
#include "IExecutionStrategy.cpp"

class ExecRet: public IExecutionStrategy
{
private:
    ExecPop* popHelper;

public:
    ExecRet(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers, ExecPop* helper);
    void executeInstruction(Instruction instr) override;
    void log(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true) override; 
    ~ExecRet();
};