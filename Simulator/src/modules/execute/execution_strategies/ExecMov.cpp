#include "ExecMov.h"

void ExecMov::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  Instruction instr = instrPackage.data;
  recorder->modifyModuleState(EX, "Executing " + instr.toString());
  SynchronizedDataPackage<std::vector<word>> movedValuePckg = getFinalArgValue(instr.src2, instr.param2, isZReg(instr.src1));

  if (movedValuePckg.exceptionTriggered)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      movedValuePckg.excpData,
      MISALIGNED_ACCESS_HANDL));
    return;
  }

  SynchronizedDataPackage<word> storeResultPckg = storeResultAtDest(movedValuePckg.data,instr.src1, instr.param1);
  
  if (storeResultPckg.exceptionTriggered)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      storeResultPckg.excpData,
      MISALIGNED_ACCESS_HANDL));
    return;
  }
  
  clock_time lastTick = refToEX->waitTillLastTick();
  moveIP(instr);
}
