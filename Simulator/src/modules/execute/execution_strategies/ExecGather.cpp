#include "ExecGather.h"

void ExecGather::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  SynchronizedDataPackage<std::vector<word>> retrievedWordPckg;
  recorder->modifyModuleState(EX, "Executing " + instrPackage.data.toString());
  address targetAddr;
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
  }
  recorder->modifyZRegister(instrPackage.data.src1 - Z0, *regs->zRegisters[instrPackage.data.src1 - Z0]);
  clock_time lastTick = refToEX->waitTillLastTick();
  moveIP(instrPackage.data);
}