#pragma once

#include "IExecutionStrategy.cpp"

class ExecCmp: public IExecutionStrategy
{
public:
    ExecCmp(InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithLS, CPURegisters* registers);
    void executeInstruction(Instruction instr) override;
    void log(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true) override;
    ~ExecCmp();
};