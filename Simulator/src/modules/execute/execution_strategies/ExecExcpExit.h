#pragma once

#include "../../../interfaces/IExecutionStrategy.h"

class ExecExcpExit: public IExecutionStrategy
{
private:
  std::string log(LoggablePackage toLog) override; 

public:
  ExecExcpExit(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers);
  void executeInstruction(SynchronizedDataPackage<Instruction> instrPackage) override;
};