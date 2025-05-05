#include "ExecScatter.h"

ExecScatter::ExecScatter(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
  IClockBoundModule* refToEX,
  std::shared_ptr<CPURegisters> registers):
    IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecScatter::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  SynchronizedDataPackage<std::vector<word>> storedWordPckg;
  word toStore;
  address targetAddr;
  std::string resultMsg = "Finished executing: " + plainInstructionToString(instrPackage.data) + " (";
  for (byte wordInd = 0; wordInd < WORDS_PER_Z_REGISTER; ++wordInd)
  {
    targetAddr = (*regs->zRegisters[instrPackage.data.src1 - Z0])[wordInd];
    toStore = (*regs->zRegisters[instrPackage.data.src2 - Z0])[wordInd];
    storedWordPckg = storeDataAt(targetAddr, 1, std::vector<word> { toStore });
    if (storedWordPckg.exceptionTriggered)
    {
      handleException(SynchronizedDataPackage<Instruction>(instrPackage.associatedIP,
        targetAddr,
        storedWordPckg.handlerAddr));
      return;
    }
    resultMsg += " " + plainArgToString(ADDR, targetAddr) + " = " + convDecToHex(toStore);
  }
  resultMsg += ")\n";
  clock_time lastTick = refToEX->waitTillLastTick();
  logComplete(lastTick, resultMsg);
  moveIP(instrPackage.data);
}