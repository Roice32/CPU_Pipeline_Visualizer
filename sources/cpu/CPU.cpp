#include "CPU.h"

#include <thread>

CPU::CPU(std::shared_ptr<Memory> memory): memoryUnit(memory)
{
    registers = std::make_shared<CPURegisters>();

    fromICtoLS = std::make_shared<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>>();
    fromICtoDE = std::make_shared<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, address>>();
    fromDEtoEX = std::make_shared<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>>();
    fromEXtoLS = std::make_shared<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>>();

    clock = std::make_shared<Clock>();
    
    LSModule = std::make_shared<LoadStore>(memory, fromICtoLS, fromEXtoLS, clock->clockSyncVars);
    ICModule = std::make_shared<InstructionCache>(fromICtoLS, fromICtoDE, clock->clockSyncVars, registers->IP);
    DEModule = std::make_shared<Decode>(fromICtoDE, fromDEtoEX, clock->clockSyncVars);
    EXModule = std::make_shared<Execute>(fromEXtoLS, fromDEtoEX, registers, clock->clockSyncVars);
}

void CPU::run()
{
    std::thread clkThread(&Clock::run, clock);
    std::thread lsThread(&LoadStore::run, LSModule);
    std::thread icThread(&InstructionCache::run, ICModule);
    std::thread deThread(&Decode::run, DEModule);
    std::thread exThread(&Execute::run, EXModule);

    lsThread.join();
    icThread.join();
    deThread.join();
    exThread.join();
    clkThread.join();
}