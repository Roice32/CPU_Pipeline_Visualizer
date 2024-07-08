#pragma once

#include "LoadStore.h"
#include "InstructionCache.h"
#include "Execute.h"
#include "Config.h"

class CPU
{
private:
    Memory* memoryUnit;
    LoadStore* LSModule;
    InstructionCache* ICModule;
    Decode* DEModule;
    Execute* EXModule;

    register_16b registers[8];
    register_16b IP;
    register_16b flags;
    register_16b stackBase;
    register_16b stackSize;
    register_16b stackPointer;

    void reset();
public:
    CPU(Memory* memory);
    void run();
    ~CPU();
};