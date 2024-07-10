#pragma once

#include "Config.h"

class CPURegisters
{
public:
    register_16b registers[REGISTER_COUNT];
    register_16b IP;
    register_16b flags;
    register_16b stackBase;
    register_16b stackSize;
    register_16b stackPointer;

    CPURegisters();
    void reset();
};