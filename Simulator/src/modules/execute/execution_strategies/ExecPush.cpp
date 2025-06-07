#include "ExecPush.h"

void ExecPush::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  Instruction instr = instrPackage.data;
  recorder->modifyModuleState(EX, "Executing " + instr.toString());
  SynchronizedDataPackage<std::vector<word>> actualParamPckg = getFinalArgValue(instr.src1, instr.param1);

  if (actualParamPckg.exceptionTriggered)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      actualParamPckg.excpData,
      MISALIGNED_ACCESS_HANDL));
    return;
  }

  if(*regs->stackSize < *regs->stackPointer || *regs->stackPointer < WORD_BYTES)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      PUSH_OVERFLOW,
      STACK_OVERFLOW_HANDL));
    return;
  }

  *regs->stackPointer -= WORD_BYTES;
  recorder->modifyStackPointer(*regs->stackPointer);
  word newSP = *regs->stackBase + *regs->stackPointer;

  SynchronizedDataPackage<std::vector<word>> storeResultPckg = storeDataAt(newSP, 1, std::vector<word> { actualParamPckg.data });

  if (storeResultPckg.exceptionTriggered)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      storeResultPckg.excpData,
      MISALIGNED_ACCESS_HANDL));
    return;
  }
  else
  {
    recorder->pushToStack(actualParamPckg.data);
  }

  moveIP(instr);
  clock_time lastTick = refToEX->waitTillLastTick();
}