#include "CPU.h"

#include <thread>

CPU::CPU(std::shared_ptr<Memory> memory, std::shared_ptr<ExecutionRecorder> recorder): memoryUnit(memory)
{
  registers = std::make_shared<CPURegisters>();
  fromICtoLS = std::make_shared<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>>();
  fromICtoDE = std::make_shared<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, SynchronizedDataPackage<address>>>();
  fromDEtoEX = std::make_shared<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>>();
  fromEXtoLS = std::make_shared<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>>();

  clock = std::make_shared<Clock>(recorder);
  
  LSModule = std::make_shared<LoadStore>(memory,
                                         fromICtoLS,
                                         fromEXtoLS,
                                         clock->clockSyncVars,
                                         recorder);
  ICModule = std::make_shared<InstructionCache>(fromICtoLS,
                                                fromICtoDE,
                                                clock->clockSyncVars,
                                                registers->IP,
                                                recorder);
  DEModule = std::make_shared<Decode>(fromICtoDE,
                                      fromDEtoEX,
                                      clock->clockSyncVars,
                                      registers->flags,
                                      recorder);
  EXModule = std::make_shared<Execute>(fromEXtoLS,
                                       fromDEtoEX,
                                       registers,
                                       clock->clockSyncVars,
                                       recorder);
}

void CPU::runSimulation()
{
  std::thread clkThread(&Clock::run, clock);
  std::thread exThread(&Execute::run, EXModule);
  std::thread deThread(&Decode::run, DEModule);
  std::thread lsThread(&LoadStore::run, LSModule);
  std::thread icThread(&InstructionCache::run, ICModule);

  exThread.join();
  deThread.join();
  icThread.join();
  lsThread.join();

  *clock->selfRunning = false;

  clkThread.join();
}