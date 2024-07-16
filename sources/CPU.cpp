#include "CPU.h"
#include "Decode.h"

#include <thread>

CPU::CPU(std::shared_ptr<Memory> memory): memoryUnit(memory)
{
    registers = std::make_shared<CPURegisters>();

    ICtoLS = std::make_shared<InterThreadCommPipe<address, fetch_window>>();
    DEtoIC = std::make_shared<InterThreadCommPipe<address, fetch_window>>();
    EXtoDE = std::make_shared<InterThreadCommPipe<address, Instruction>>();
    EXtoLS = std::make_shared<InterThreadCommPipe<MemoryAccessRequest, word>>();

    clock = std::make_shared<Clock>(registers->flags);
    
    LSModule = std::make_shared<LoadStore>(memory, ICtoLS, EXtoLS, registers->flags);
    ICModule = std::make_shared<InstructionCache>(ICtoLS, DEtoIC, registers->flags);
    DEModule = std::make_shared<Decode>(DEtoIC, EXtoDE, registers->flags);
    EXModule = std::make_shared<Execute>(EXtoLS, EXtoDE, registers, clock->clockSyncVars);

    *registers->flags |= RUNNING;
}

void CPU::run()
{
    std::thread lsThread(&LoadStore::run, LSModule);
    std::thread icThread(&InstructionCache::run, ICModule);
    std::thread deThread(&Decode::run, DEModule);
    std::thread exThread(&Execute::run, EXModule);
    std::thread clkThread(&Clock::run, clock);

    lsThread.join();
    icThread.join();
    deThread.join();
    exThread.join();
    clkThread.join();
}