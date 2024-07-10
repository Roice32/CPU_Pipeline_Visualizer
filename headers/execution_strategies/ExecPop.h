#pragma once

#include "IExecutionStrategy.cpp"

class ExecPop: public IExecutionStrategy
{
public:
    ExecPop(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    void executeInstruction(Instruction instr);
    void executeInstructionNoLog(Instruction instr);
};