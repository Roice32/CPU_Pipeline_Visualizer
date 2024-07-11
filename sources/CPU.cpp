#include "CPU.h"
#include "Decode.h"

#include <thread>

CPU::CPU(Memory* memory): memoryUnit(memory)
{
    ICtoLS = new InterThreadCommPipe<address, fetch_window>();

    registers = new CPURegisters();
    LSModule = new LoadStore(memory, ICtoLS, &registers->flags);
    ICModule = new InstructionCache(ICtoLS, &registers->IP);
    DEModule = new Decode(&registers->IP);
    EXModule = new Execute(LSModule, registers, ICModule);
    ICModule->setDEModule(DEModule);
    DEModule->setEXModule(EXModule);

    registers->flags |= RUNNING;
}

void CPU::run()
{
    std::thread lsThread(&LoadStore::run, LSModule);
    ICModule->requestFetchWindow();
    lsThread.join();
}

CPU::~CPU()
{
    delete ICtoLS;

    delete LSModule;
    delete ICModule;
    delete DEModule;
    delete EXModule;
}