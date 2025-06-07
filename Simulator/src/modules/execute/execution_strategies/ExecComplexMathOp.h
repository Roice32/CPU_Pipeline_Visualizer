#pragma once

#include "IExecutionStrategy.h"

class ExecComplexMathOp: public IExecutionStrategy
{
private:
  bool handleNormalComplexOp(Instruction instr, word actualParam1, word actualParam2);
  bool handleZRegComplexOp(Instruction instr, std::vector<word> actualParam1, std::vector<word> actualParam2);

public:
  ExecComplexMathOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ExecutionRecorder> recorder) :
      IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers, recorder) {};

  void executeInstruction(SynchronizedDataPackage<Instruction> instrPackage) override;
};