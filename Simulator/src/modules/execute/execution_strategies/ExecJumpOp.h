#pragma once

#include "IExecutionStrategy.h"

class ExecJumpOp: public IExecutionStrategy
{
public:
  ExecJumpOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> fromDEtoMe,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ExecutionRecorder> recorder) :
      IExecutionStrategy(commPipeWithLS, fromDEtoMe, refToEX, registers, recorder) {};

  void executeInstruction(SynchronizedDataPackage<Instruction> instrPackage) override;
};