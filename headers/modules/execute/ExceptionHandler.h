#pragma once

#include "IExecutionStrategy.cpp"

class ExceptionHandler: public IExecutionStrategy
{
private:
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> fromDEtoMe;

    void executeInstruction(Instruction instr) override;
    std::string logReceived(SynchronizedDataPackage<Instruction> faultyInstr);
    std::string log(LoggablePackage toLog) override;

public:
    ExceptionHandler(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
        IClockBoundModule* refToEX,
        std::shared_ptr<CPURegisters> registers);
    void handleException(SynchronizedDataPackage<Instruction> faultyInstr);
};