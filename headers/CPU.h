#pragma once

#include "CPURegisters.h"
#include "LoadStore.h"
#include "InstructionCache.h"
#include "Decode.h"
#include "Execute.h"
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

    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> ICtoLS;
    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> DEtoIC;
    std::shared_ptr<InterThreadCommPipe<address, Instruction>> EXtoDE;
    std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> EXtoLS;

public:
    CPU(std::shared_ptr<Memory> memory);
    void run();
};