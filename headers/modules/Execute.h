#pragma once

#include "CPURegisters.h"
#include "Instruction.h"
#include "InstructionCache.h"
#include "IMemoryAccesser.cpp"

class Execute: public IMemoryAccesser
{
private:
    CPURegisters* registers;
    InstructionCache* ICModule;

public:
    Execute(LoadStore* lsModule, CPURegisters* registers, InstructionCache* icModule);
    word requestDataAt(word addr);
    void storeDataAt(word addr, word data);
    void executeInstruction(Instruction instr);

    friend class IExecutionStrategy;
};