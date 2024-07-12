#pragma once

#include "CPURegisters.h"
#include "Instruction.h"
#include "IExecutionStrategy.cpp"

class Execute
{
private:
    InterThreadCommPipe<MemoryAccessRequest, word>* requestsToLS;
    InterThreadCommPipe<byte, Instruction>* requestsToDE;
    std::unordered_map<OpCode, IExecutionStrategy*> execStrategies;
    CPURegisters* registers;

public:
    Execute(InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithLS, InterThreadCommPipe<byte, Instruction>* commPipeWithDE, CPURegisters* registers);
    word requestDataAt(word addr);
    void storeDataAt(word addr, word data);
    void executeInstruction(Instruction instr);
    void run();
    ~Execute();

    friend class IExecutionStrategy;
};