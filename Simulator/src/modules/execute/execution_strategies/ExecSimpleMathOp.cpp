#include "ExecSimpleMathOp.h"

void ExecSimpleMathOp::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  Instruction instr = instrPackage.data;
  recorder->modifyModuleState(EX, "Executing: " + instr.toString());

  bool zRegInvolved = isZReg(instr.src1) || isZReg(instr.src2);
  SynchronizedDataPackage<std::vector<word>> actualParam1Pckg = getFinalArgValue(instr.src1, instr.param1, zRegInvolved);
  SynchronizedDataPackage<std::vector<word>> actualParam2Pckg = getFinalArgValue(instr.src2, instr.param2, zRegInvolved);

  if (actualParam1Pckg.exceptionTriggered || actualParam2Pckg.exceptionTriggered)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      (actualParam1Pckg.exceptionTriggered ? actualParam1Pckg.excpData : actualParam2Pckg.excpData),
      MISALIGNED_ACCESS_HANDL));
    return;
  }

  std::vector<word> result;
  if (zRegInvolved)
    for (byte wordInd = 0; wordInd < WORDS_PER_Z_REGISTER; ++wordInd)
    {
      result.push_back(instr.opCode == ADD ?
        actualParam1Pckg.data[wordInd] + actualParam2Pckg.data[wordInd] :
        actualParam1Pckg.data[wordInd] - actualParam2Pckg.data[wordInd]);
      if (result[wordInd] == 0)
      {
        *regs->flags |= ZERO;
        recorder->modifyFlags(*regs->flags);
      }
    }
  else
  {
    result.push_back( instr.opCode == ADD ?
      actualParam1Pckg.data[0] + actualParam2Pckg.data[0] :
      actualParam1Pckg.data[0] - actualParam2Pckg.data[0]);
    if (result[0] == 0)
    {
      *regs->flags |= ZERO;
      recorder->modifyFlags(*regs->flags);
    }
  }
  storeResultAtDest(result, instr.src1, instr.param1);
  moveIP(instr);
  clock_time lastTick = refToEX->waitTillLastTick();
}
