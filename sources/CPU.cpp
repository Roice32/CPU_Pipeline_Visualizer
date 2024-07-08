#include "CPU.h"

#include "Decode.h"

CPU::CPU(Memory* memory): memoryUnit(memory)
{
    LSModule = new LoadStore(memory);
    ICModule = new InstructionCache(LSModule, &IP);
    DEModule = new Decode(&IP);
    EXModule = new Execute(LSModule, &IP, ICModule);
    ICModule->setDEModule(DEModule);
    DEModule->setEXModule(EXModule);

    reset();
}

void CPU::reset()
{
    for (byte reg = 0; reg < 8; ++reg)
        registers[reg] = 0;
    IP = 0xfff0;
    stackBase = stackPointer = 0xffed;
    stackSize = 128; 
}

void CPU::run() { ICModule->requestFetchWindow(); }

CPU::~CPU()
{
    // delete LSModule; This won't work for some reason.
    delete ICModule;
    delete DEModule;
    delete EXModule;
}