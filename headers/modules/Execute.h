#pragma once

#include "CPURegisters.h"
#include "Instruction.h"
#include "IExecutionStrategy.cpp"

#include <memory>

class Execute
{
private:
    std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> requestsToLS;
    std::shared_ptr<InterThreadCommPipe<byte, Instruction>> requestsToDE;
    std::unordered_map<OpCode, std::shared_ptr<IExecutionStrategy>> execStrategies;
    std::shared_ptr<CPURegisters> registers;

    word requestDataAt(word addr);
    void storeDataAt(word addr, word data);
    void executeInstruction(Instruction instr);

public:
    Execute(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS, std::shared_ptr<InterThreadCommPipe<byte, Instruction>> commPipeWithDE, std::shared_ptr<CPURegisters> registers);
    void run();

    //friend class IExecutionStrategy;
};