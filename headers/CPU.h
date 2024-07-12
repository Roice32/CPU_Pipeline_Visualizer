#pragma once

#include "CPURegisters.h"
#include "LoadStore.h"
#include "InstructionCache.h"
#include "Execute.h"

class CPU
{
private:
    Memory* memoryUnit;
    CPURegisters* registers;
    
    LoadStore* LSModule;
    InstructionCache* ICModule;
    Decode* DEModule;
    Execute* EXModule;

    InterThreadCommPipe<address, fetch_window>* ICtoLS;
    InterThreadCommPipe<address, fetch_window>* DEtoIC;
    InterThreadCommPipe<MemoryAccessRequest, word>* EXtoLS;

public:
    CPU(Memory* memory);
    void run();
    ~CPU();
};