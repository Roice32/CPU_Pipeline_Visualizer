#pragma once

#include "CPURegisters.h"
#include "Instruction.h"
#include "InstructionCache.h"
#include "ExecutionStrategyPool.h"

class Execute// Need to get rid of this inheritance
{
private:
    CPURegisters* registers;
    InstructionCache* ICModule;
    ExecutionStrategyPool* execStrategies;

public:
    Execute(LoadStore* lsModule, CPURegisters* registers, InstructionCache* icModule);
    word requestDataAt(word addr);
    void storeDataAt(word addr, word data);
    void executeInstruction(Instruction instr);

    friend class IExecutionStrategy;
};