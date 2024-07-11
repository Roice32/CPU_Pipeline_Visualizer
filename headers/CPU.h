#pragma once

#include "CPURegisters.h"
#include "CommQueue.h"
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

    CommQueue<fetch_window> LStoIC;
    CommQueue<fetch_window> ICtoDE;
    CommQueue<Instruction> DEtoEX;
    CommQueue<word> EXtoLS;
    CommQueue<word> LStoEX;

public:
    CPU(Memory* memory);
    void run();
    ~CPU();
};