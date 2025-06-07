#include "ExecPop.h"

void ExecPop::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  Instruction instr = instrPackage.data;
  recorder->modifyModuleState(EX, "Executing " + instr.toString());

  if (*regs->stackSize < *regs->stackPointer || *regs->stackSize - *regs->stackPointer < WORD_BYTES)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      POP_OVERFLOW,
      STACK_OVERFLOW_HANDL));
    return;
  }

  SynchronizedDataPackage<std::vector<word>> valueOnTopPckg;
  if (instr.src1 != NULL_VAL)
  {
    word topOfStack = *regs->stackBase + *regs->stackPointer;
    valueOnTopPckg = requestDataAt(topOfStack, 1);

    if (valueOnTopPckg.exceptionTriggered)
    {
      handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
        valueOnTopPckg.excpData, MISALIGNED_ACCESS_HANDL));
      return;
    }

    SynchronizedDataPackage<word> storeResultPckg = storeResultAtDest(valueOnTopPckg.data,instr.src1, instr.param1);

    if (storeResultPckg.exceptionTriggered)
    {
      handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
        storeResultPckg.excpData,
        MISALIGNED_ACCESS_HANDL));
      return;
    }
  }
  *regs->stackPointer += WORD_BYTES;
  recorder->modifyStackPointer(*regs->stackPointer);
  recorder->popFromStack();
  moveIP(instr);
  clock_time lastTick = refToEX->waitTillLastTick();
}