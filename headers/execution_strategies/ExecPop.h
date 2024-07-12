#pragma once

#include "IExecutionStrategy.cpp"

class ExecPop: public IExecutionStrategy
{
public:
    ExecPop(InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithLS, CPURegisters* registers);
    void executeInstruction(Instruction instr) override;
    void executeInstructionNoLog(Instruction instr);
    ~ExecPop();
};