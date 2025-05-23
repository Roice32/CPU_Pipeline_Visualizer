#pragma once

#include "IExecutionStrategy.h"

class ExecJumpOp: public IExecutionStrategy
{
public:
  ExecJumpOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> fromDEtoMe,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers);
  void executeInstruction(SynchronizedDataPackage<Instruction> instrPackage) override;
  std::string log(LoggablePackage toLog) override;
};