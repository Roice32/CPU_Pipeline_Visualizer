#pragma once

#include "CPURegisters.h"
#include "Instruction.h"
#include "IClockBoundModule.cpp"
#include "IExecutionStrategy.cpp"

#include <memory>

class Execute: public IClockBoundModule
{
private:
    std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> requestsToLS;
    std::shared_ptr<InterThreadCommPipe<address, Instruction>> requestsToDE;
    std::unordered_map<OpCode, std::shared_ptr<IExecutionStrategy>> execStrategies;
    std::shared_ptr<CPURegisters> registers;

    word requestDataAt(word addr);
    void storeDataAt(word addr, word data);
    void executeInstruction(Instruction instr);

public:
    Execute(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS,
        std::shared_ptr<InterThreadCommPipe<address, Instruction>> commPipeWithDE,
        std::shared_ptr<CPURegisters> registers,
        std::shared_ptr<ClockSyncPackage> clockSyncVars);
    bool executeModuleLogic() override;
};