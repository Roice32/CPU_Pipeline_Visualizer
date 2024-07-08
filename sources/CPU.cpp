#include "CPU.h"

#include "Decode.h"

CPU::CPU(Memory* memory): memoryUnit(memory)
{

    LSModule = new LoadStore(memory);
    ICModule = new InstructionCache(LSModule, &registers->IP);
    DEModule = new Decode(&registers->IP);
    EXModule = new Execute(LSModule, registers, ICModule);
    ICModule->setDEModule(DEModule);
    DEModule->setEXModule(EXModule);
}

void CPU::run() { ICModule->requestFetchWindow(); }

CPU::~CPU()
{
    // delete LSModule; This won't work for some reason.
    delete ICModule;
    delete DEModule;
    delete EXModule;
}