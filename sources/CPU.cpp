#include "CPU.h"
#include "Decode.h"

#include <thread>

CPU::CPU(Memory* memory): memoryUnit(memory)
{
    ICtoLS = new InterThreadCommPipe<address, fetch_window>();
    DEtoIC = new InterThreadCommPipe<address, fetch_window>();

    registers = new CPURegisters();
    LSModule = new LoadStore(memory, ICtoLS, &registers->flags);
    ICModule = new InstructionCache(ICtoLS, DEtoIC, &registers->flags);
    DEModule = new Decode(DEtoIC, &registers->IP);
    EXModule = new Execute(LSModule, registers, ICModule);
    DEModule->setEXModule(EXModule);

    registers->flags |= RUNNING;
}

void CPU::run()
{
    std::thread lsThread(&LoadStore::run, LSModule);
    std::thread icThread(&InstructionCache::run, ICModule);
    DEModule->run();
    lsThread.join();
    icThread.join();
}

CPU::~CPU()
{
    delete ICtoLS;
    delete DEtoIC;

    delete LSModule;
    delete ICModule;
    delete DEModule;
    delete EXModule;
}