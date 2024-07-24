#pragma once

#include "GeneralExceptionHandler.h"

class Execute: public IClockBoundModule, public EXLogger
{
private:
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> fromMeToLS;
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> fromDEtoMe;
    std::unordered_map<OpCode, std::shared_ptr<IExecutionStrategy>> execStrategies;
    std::shared_ptr<CPURegisters> registers;
    GeneralExceptionHandler exceptionHandler;

    word requestDataAt(word addr);
    void storeDataAt(word addr, word data);
    void executeInstruction(SynchronizedDataPackage<Instruction> instr);

public:
    Execute(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
        std::shared_ptr<CPURegisters> registers,
        std::shared_ptr<ClockSyncPackage> clockSyncVars);
    bool executeModuleLogic() override;
};