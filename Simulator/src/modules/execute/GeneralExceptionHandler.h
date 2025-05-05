#pragma once

#include "../../../interfaces/IExecutionStrategy.cpp"

class GeneralExceptionHandler: public IExecutionStrategy
{
public:
    GeneralExceptionHandler(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
        IClockBoundModule* refToEX,
        std::shared_ptr<CPURegisters> registers):
            IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};
    void executeInstruction(SynchronizedDataPackage<Instruction> faultyInstr) override {};
};