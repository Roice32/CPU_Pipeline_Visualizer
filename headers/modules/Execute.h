#pragma once

#include "Instruction.h"
#include "InstructionCache.h"
#include "IMemoryAccesser.cpp"

class Execute: public IMemoryAccesser
{
private:
    register_16b* IP;
    InstructionCache* ICModule;

public:
    Execute(LoadStore* lsModule, register_16b* const ip, InstructionCache* icModule);
    word requestDataAt(word addr);
    void storeDataAt(word addr, word data);
    void executeInstruction(Instruction instr);
};