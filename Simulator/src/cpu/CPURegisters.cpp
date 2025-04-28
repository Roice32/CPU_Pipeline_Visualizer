#include "CPURegisters.h"
#include <cassert>

CPURegisters::CPURegisters()
{
    assert(REGISTER_COUNT > 0 && "REGISTER_COUNT must be positive integer");
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        registers[reg] = std::make_shared<register_16b>();
    for (byte zReg = 0; zReg < Z_REGISTER_COUNT; ++zReg)
        zRegisters[zReg] = std::make_shared<std::vector<word>>();

    IP = std::make_shared<register_16b>();
    flags = std::make_shared<register_16b>();
    stackBase = std::make_shared<register_16b>();
    stackSize = std::make_shared<register_16b>();
    stackPointer = std::make_shared<register_16b>();
    reset();
}

void CPURegisters::reset()
{
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        *registers[reg] = 0;
    for (byte zReg = 0; zReg < Z_REGISTER_COUNT; ++zReg)
    {
        zRegisters[zReg]->clear();
        for (byte wordInd = 0; wordInd < WORDS_PER_Z_REGISTER; ++wordInd)
            zRegisters[zReg]->push_back(0);
    }
    *IP = 0xfff0;
    *stackBase = 0xefee;
    *stackPointer = 4096;
    *stackSize = 4096;
    *flags = 0;
}