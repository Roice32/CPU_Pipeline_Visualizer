#pragma once

#include "IExecutionStrategy.h"

class ExecScatter: public IExecutionStrategy
{
public:
  ExecScatter(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ExecutionRecorder> recorder) :
      IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers, recorder) {};

  void executeInstruction(SynchronizedDataPackage<Instruction> instrPackage) override;
};