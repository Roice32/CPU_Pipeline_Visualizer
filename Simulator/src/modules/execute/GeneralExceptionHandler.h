#pragma once

#include "IExecutionStrategy.h"

class GeneralExceptionHandler: public IExecutionStrategy
{
public:
  GeneralExceptionHandler(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ExecutionRecorder> recorder) :
      IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers, recorder) {};
  void executeInstruction(SynchronizedDataPackage<Instruction> faultyInstr) override {};
};