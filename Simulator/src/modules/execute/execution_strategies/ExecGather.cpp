#include "ExecGather.h"

ExecGather::ExecGather(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
  IClockBoundModule* refToEX,
  std::shared_ptr<CPURegisters> registers):
    IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecGather::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  SynchronizedDataPackage<std::vector<word>> retrievedWordPckg;
  address targetAddr;
  std::string resultMsg = "Finished executing: " + plainInstructionToString(instrPackage.data) + " (";
  resultMsg += typeNames.at((TypeCode) instrPackage.data.src1);
  resultMsg += " =";
  for (byte wordInd = 0; wordInd < WORDS_PER_Z_REGISTER; ++wordInd)
  {
    targetAddr = (*regs->zRegisters[instrPackage.data.src2 - Z0])[wordInd];
    retrievedWordPckg = requestDataAt( targetAddr, 1);
    if (retrievedWordPckg.exceptionTriggered)
    {
      handleException(SynchronizedDataPackage<Instruction>(instrPackage.associatedIP,
        targetAddr,
        retrievedWordPckg.handlerAddr));
      return;
    }
    (*regs->zRegisters[instrPackage.data.src1 - Z0])[wordInd] = retrievedWordPckg.data[0];
    resultMsg += " " + convDecToHex(retrievedWordPckg.data[0]);
  }
  resultMsg += ")\n";
  clock_time lastTick = refToEX->waitTillLastTick();
  logComplete(lastTick, resultMsg);
  moveIP(instrPackage.data);
}