#pragma once

#include "CPURegisters.h"
#include "Instruction.h"
#include "InstructionCache.h"
#include "IExecutionStrategy.cpp"

class Execute
{
private:
    CPURegisters* registers;
    InstructionCache* ICModule;
    std::unordered_map<OpCode, IExecutionStrategy*> execStrategies;

public:
    Execute(LoadStore* lsModule, CPURegisters* registers, InstructionCache* icModule);
    word requestDataAt(word addr);
    void storeDataAt(word addr, word data);
    void executeInstruction(Instruction instr);
    ~Execute();

    friend class IExecutionStrategy;
};