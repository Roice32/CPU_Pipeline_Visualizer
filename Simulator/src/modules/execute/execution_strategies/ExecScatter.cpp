#include "ExecScatter.h"

void ExecScatter::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  Instruction instr = instrPackage.data;
  recorder->modifyModuleState(EX, "Executing: " + instr.toString());

  SynchronizedDataPackage<std::vector<word>> storedWordPckg;
  word toStore;
  address targetAddr;

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
  }

  clock_time lastTick = refToEX->waitTillLastTick();
  moveIP(instrPackage.data);
}