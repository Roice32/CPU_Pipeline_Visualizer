#pragma once

#include "GeneralExceptionHandler.h"

class Execute: public IClockBoundModule
{
private:
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> fromMeToLS;
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> fromDEtoMe;
  std::unordered_map<OpCode, std::shared_ptr<IExecutionStrategy>> execStrategies;
  std::shared_ptr<CPURegisters> registers;
  GeneralExceptionHandler exceptionHandler;
  std::shared_ptr<ExecutionRecorder> recorder;

  void executeInstruction(SynchronizedDataPackage<Instruction> instr);

public:
  Execute(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ClockSyncPackage> clockSyncVars,
    std::shared_ptr<ExecutionRecorder> recorder);
  void executeModuleLogic() override;
};