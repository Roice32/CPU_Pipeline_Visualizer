#include "ExecCall.h"

void ExecCall::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  Instruction instr = instrPackage.data;
  recorder->modifyModuleState(EX, "Executing " + instr.toString());

  if (*regs->stackSize < *regs->stackPointer || *regs->stackPointer < (REGISTER_COUNT + 2) * WORD_BYTES)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      PUSH_OVERFLOW,
      STACK_OVERFLOW_HANDL));
    return;
  }

  SynchronizedDataPackage<std::vector<word>> methodAddressPckg = getFinalArgValue(instr.src1, instr.param1);
  if (methodAddressPckg.exceptionTriggered)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      methodAddressPckg.excpData,
      MISALIGNED_ACCESS_HANDL));
    return;
  }
  
  std::vector<word> savedState;
  for (byte reg = REGISTER_COUNT - 1; reg < REGISTER_COUNT; --reg)
    savedState.push_back(*regs->registers[reg]);
  savedState.push_back(*regs->flags);
  savedState.push_back(*regs->IP + 2 * WORD_BYTES);
  *regs->stackPointer -= (REGISTER_COUNT + 2) * WORD_BYTES;

  std::vector<address> savedStateAddrs;
  for (int ind = 0; ind < savedState.size(); ++ind)
    savedStateAddrs.push_back(*regs->stackBase + *regs->stackPointer + ind * WORD_BYTES);

  SynchronizedDataPackage<std::vector<word>> storeResultPckg = storeDataAt(*regs->stackBase + *regs->stackPointer,
                                                                           REGISTER_COUNT + 2,
                                                                           savedState);
  recorder->pushToStack(savedState, true);
  SynchronizedDataPackage<address> mssgToDE(methodAddressPckg.data[0]);
  clock_time lastTick = refToEX->waitTillLastTick();
  if (storeResultPckg.exceptionTriggered)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      storeResultPckg.excpData,
      MISALIGNED_ACCESS_HANDL));
    return;
  }
  else
  {
    recorder->addExtraInfo(EX, "Saved {R7-0, FLAGS, NextIP} at memory range [#"
                                + convDecToHex(*regs->stackBase + *regs->stackPointer)
                                + " - #"
                                + convDecToHex(*regs->stackBase + *regs->stackPointer + (REGISTER_COUNT + 2) * WORD_BYTES - 1)
                                + "]");
    recorder->modifyStackPointer(*regs->stackPointer);
  }
  jumpIP(methodAddressPckg.data[0]);
  mssgToDE.sentAt = lastTick;
  fromDEtoMe->sendB(mssgToDE);
  recorder->pushEXtoDEData(mssgToDE);
}
