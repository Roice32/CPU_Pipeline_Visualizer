#include "ExecRet.h"

void ExecRet::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  Instruction instr = instrPackage.data;
  recorder->modifyModuleState(EX, "Executing: " + instr.toString());

  if(*regs->stackSize < *regs->stackPointer || *regs->stackSize - *regs->stackPointer < (REGISTER_COUNT + 2) * WORD_BYTES)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      POP_OVERFLOW,
      STACK_OVERFLOW_HANDL));
    return;
  }

  SynchronizedDataPackage<std::vector<word>> restoredStatePckg = requestDataAt(*regs->stackBase + *regs->stackPointer, REGISTER_COUNT + 2);

  if (restoredStatePckg.exceptionTriggered)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      restoredStatePckg.excpData,
      MISALIGNED_ACCESS_HANDL));
    return;
  }

  recorder->popFromStack(REGISTER_COUNT + 2);

  for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
  {
    *regs->registers[REGISTER_COUNT - 1 - reg] = restoredStatePckg.data[reg];
    recorder->modifyRRegister(REGISTER_COUNT - 1 - reg, *regs->registers[REGISTER_COUNT - 1 - reg]);
  }

  *regs->flags = restoredStatePckg.data[REGISTER_COUNT];
  recorder->modifyFlags(*regs->flags);

  *regs->IP = restoredStatePckg.data[REGISTER_COUNT + 1];
  recorder->modifyIP(*regs->IP);
  
  *regs->stackPointer += (REGISTER_COUNT + 2) * WORD_BYTES;
  recorder->modifyStackPointer(*regs->stackPointer);

  SynchronizedDataPackage<address> mssgToDE(restoredStatePckg.data[REGISTER_COUNT + 1]);
  clock_time lastTick = refToEX->waitTillLastTick();
  mssgToDE.sentAt = lastTick;
  fromDEtoMe->sendB(mssgToDE);
  recorder->pushEXtoDEData(mssgToDE);
}
