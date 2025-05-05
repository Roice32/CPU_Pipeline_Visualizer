#pragma once

#include "../modules/load_store/LoadStore.h"
#include "../modules/instruction_cache/InstructionCache.h"
#include "../modules/decode/Decode.h"
#include "../modules/execute/Execute.h"
#include "Clock.h"

class CPU
{
private:
  std::shared_ptr<Memory> memoryUnit;
  std::shared_ptr<CPURegisters> registers;
  
  std::shared_ptr<LoadStore> LSModule;
  std::shared_ptr<InstructionCache> ICModule;
  std::shared_ptr<Decode> DEModule;
  std::shared_ptr<Execute> EXModule;

  std::shared_ptr<Clock> clock;

  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> fromICtoLS;
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, SynchronizedDataPackage<address>>> fromICtoDE;
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> fromDEtoEX;
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> fromEXtoLS;

public:
  CPU(std::shared_ptr<Memory> memory);
  void runSimulation();
};