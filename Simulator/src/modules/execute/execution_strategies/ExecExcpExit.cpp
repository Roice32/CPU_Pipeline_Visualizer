#include "ExecExcpExit.h"

void ExecExcpExit::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  Instruction instr = instrPackage.data;
  recorder->modifyModuleState(EX, "Executing " + instr.toString());
  SynchronizedDataPackage<std::vector<word>> restoredStatePckg = requestDataAt(SAVE_STATE_ADDR, REGISTER_COUNT + 4);

  for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
  {
    *regs->registers[reg] = restoredStatePckg.data[4 + reg];
    recorder->modifyRRegister(reg, *regs->registers[reg]);
  }

  *regs->flags = restoredStatePckg.data[2];
  *regs->flags &= ~EXCEPTION;
  recorder->modifyFlags(*regs->flags);

  *regs->stackPointer = restoredStatePckg.data[1];
  recorder->modifyStackPointer(*regs->stackPointer);
  
  *regs->IP = restoredStatePckg.data[0];
  recorder->modifyIP(*regs->IP);

  SynchronizedDataPackage<address> mssgToDE(restoredStatePckg.data[0]);
  clock_time lastTick = refToEX->waitTillLastTick();
  mssgToDE.sentAt = lastTick;
  fromDEtoMe->sendB(mssgToDE);
  recorder->clearEXException();
  recorder->pushEXtoDEData(mssgToDE);
}
