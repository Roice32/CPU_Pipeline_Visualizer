#pragma once

#include "IExecutionStrategy.cpp"

class ExecPush: public IExecutionStrategy
{
public:
    ExecPush(InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithLS, CPURegisters* registers);
    void executeInstruction(Instruction instr) override;
    void executeInstructionNoLog(Instruction instr);
    ~ExecPush();
};