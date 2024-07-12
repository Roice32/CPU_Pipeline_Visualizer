#include "CPU.h"
#include "Decode.h"

#include <thread>

CPU::CPU(Memory* memory): memoryUnit(memory)
{
    ICtoLS = new InterThreadCommPipe<address, fetch_window>();
    DEtoIC = new InterThreadCommPipe<address, fetch_window>();
    EXtoDE = new InterThreadCommPipe<byte, Instruction>();
    EXtoLS = new InterThreadCommPipe<MemoryAccessRequest, word>();

    registers = new CPURegisters();
    LSModule = new LoadStore(memory, ICtoLS, EXtoLS, &registers->flags);
    ICModule = new InstructionCache(ICtoLS, DEtoIC, &registers->flags);
    DEModule = new Decode(DEtoIC, EXtoDE, &registers->IP);
    EXModule = new Execute(EXtoLS, EXtoDE, registers);

    registers->flags |= RUNNING;
}

void CPU::run()
{
    std::thread lsThread(&LoadStore::run, LSModule);
    std::thread icThread(&InstructionCache::run, ICModule);
    std::thread deThread(&Decode::run, DEModule);
    std::thread exThread(&Execute::run, EXModule);

    lsThread.join();
    icThread.join();
    deThread.join();
    exThread.join();
}

CPU::~CPU()
{
    delete ICtoLS;
    delete DEtoIC;
    delete EXtoDE;
    delete EXtoLS;

    delete LSModule;
    delete ICModule;
    delete DEModule;
    delete EXModule;
}