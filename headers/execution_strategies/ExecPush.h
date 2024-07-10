#pragma once

#include "IExecutionStrategy.cpp"

class ExecPush: public IExecutionStrategy
{
public:
    ExecPush(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    void executeInstruction(Instruction instr);
    void executeInstructionNoLog(Instruction instr);
};