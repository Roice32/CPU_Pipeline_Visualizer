#pragma once

#include "CPURegisters.h"
#include "Instruction.h"
#include "IExecutionStrategy.cpp"

class Execute
{
private:
    InterThreadCommPipe<MemoryAccessRequest, word>* requestsToLS;
    std::unordered_map<OpCode, IExecutionStrategy*> execStrategies;
    CPURegisters* registers;

public:
    Execute(InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithLS, CPURegisters* registers);
    word requestDataAt(word addr);
    void storeDataAt(word addr, word data);
    void executeInstruction(Instruction instr);
    ~Execute();

    friend class IExecutionStrategy;
};