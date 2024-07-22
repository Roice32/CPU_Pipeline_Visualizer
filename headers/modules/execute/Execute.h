#pragma once

#include "CPURegisters.h"
#include "Instruction.h"
#include "IClockBoundModule.cpp"
#include "IExecutionStrategy.cpp"
#include "SynchronizedDataPackage.h"

#include <memory>

class Execute: public IClockBoundModule, public EXLogger
{
private:
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> fromMeToLS;
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> fromDEtoMe;
    std::unordered_map<OpCode, std::shared_ptr<IExecutionStrategy>> execStrategies;
    std::shared_ptr<CPURegisters> registers;

    word requestDataAt(word addr);
    void storeDataAt(word addr, word data);
    void executeInstruction(Instruction instr);

public:
    Execute(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
        std::shared_ptr<CPURegisters> registers,
        std::shared_ptr<ClockSyncPackage> clockSyncVars);
    bool executeModuleLogic() override;
};