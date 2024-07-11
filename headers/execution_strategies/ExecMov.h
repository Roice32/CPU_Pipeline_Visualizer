#pragma once

#include "IExecutionStrategy.cpp"

class ExecMov: public IExecutionStrategy
{
public:
    ExecMov(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    void executeInstruction(Instruction instr) override;
    void log(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true) override;
    ~ExecMov();
};