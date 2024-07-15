#include "CPURegisters.h"

CPURegisters::CPURegisters()
{
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        registers[reg] = std::make_shared<register_16b>();
    
    IP = std::make_shared<register_16b>();
    flags = std::make_shared<register_16b>();
    stackBase = std::make_shared<register_16b>();
    stackSize = std::make_shared<register_16b>();
    stackPointer = std::make_shared<register_16b>();
    reset();
}

void CPURegisters::reset()
{
    for (byte reg = 0; reg < 8; ++reg)
        *registers[reg] = 0;
    *IP = 0xfff0;
    *stackBase = 0xefee;
    *stackPointer = 4096;
    *stackSize = 4096;
    *flags = 0;
}