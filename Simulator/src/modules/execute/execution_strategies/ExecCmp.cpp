#include "ExecCmp.h"

void ExecCmp::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  Instruction instr = instrPackage.data;
  recorder->modifyModuleState(EX, "Executing " + instr.toString());

  *regs->flags &= ~(ZERO | EQUAL | GREATER);

  SynchronizedDataPackage<std::vector<word>> actualParam1Pckg = getFinalArgValue(instr.src1, instr.param1);
  SynchronizedDataPackage<std::vector<word>> actualParam2Pckg = getFinalArgValue(instr.src2, instr.param2);

  if (actualParam1Pckg.exceptionTriggered || actualParam2Pckg.exceptionTriggered)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP, 
      (actualParam1Pckg.exceptionTriggered ? actualParam1Pckg.excpData : actualParam2Pckg.excpData),
      MISALIGNED_ACCESS_HANDL));
    return;
  }

  if (actualParam1Pckg.data[0] == 0 && actualParam2Pckg.data[0] == 0)
    *regs->flags |= ZERO;
  if (actualParam1Pckg.data[0] == actualParam2Pckg.data[0])
    *regs->flags |= EQUAL;
  if (actualParam1Pckg.data[0] > actualParam2Pckg.data[0])
    *regs->flags |= GREATER;
  recorder->modifyFlags(*regs->flags);
  moveIP(instr);
  clock_time lastTick = refToEX->waitTillLastTick();
}
