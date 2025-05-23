#pragma once

#include "../../../interfaces/IExecutionStrategy.h"

class ExecMov: public IExecutionStrategy
{
private:
  std::string log(LoggablePackage toLog) override;
  std::string logComplex(Instruction instr, std::vector<word> movedVal);

public:
  ExecMov(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers);
  void executeInstruction(SynchronizedDataPackage<Instruction> instrPackage) override;
};