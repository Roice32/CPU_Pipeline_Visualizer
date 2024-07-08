#include "CPURegisters.h"

CPURegisters::CPURegisters()
{
    reset();
}

void CPURegisters::reset()
{
    for (byte reg = 0; reg < 8; ++reg)
        registers[reg] = 0;
    IP = 0xfff0;
    stackBase = stackPointer = 0xffed;
    stackSize = 128; 
}