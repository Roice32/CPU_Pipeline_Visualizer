#include "ExecJumpOp.h"

void ExecJumpOp::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  Instruction instr = instrPackage.data;
  recorder->modifyModuleState(EX, "Executing " + instr.toString());
  SynchronizedDataPackage<std::vector<word>> jumpAddressPckg = getFinalArgValue(instr.src1, instr.param1);

  if (jumpAddressPckg.exceptionTriggered)
  {
    handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
      jumpAddressPckg.excpData,
      MISALIGNED_ACCESS_HANDL));
    return;
  }

  bool plainJump = (instr.opCode == JMP);
  bool equalJump = (instr.opCode == JE && (*regs->flags & EQUAL));
  bool lessJump = (instr.opCode == JL && !(*regs->flags & EQUAL) && !(*regs->flags & GREATER));
  bool greaterJump = (instr.opCode == JG && (*regs->flags & GREATER));
  bool zeroJump = (instr.opCode == JZ && (*regs->flags & ZERO));
  SynchronizedDataPackage<address> mssgToDE(jumpAddressPckg.data[0]);
  clock_time lastTick = refToEX->waitTillLastTick();
  if (plainJump || equalJump || lessJump || greaterJump || zeroJump)
  {
    recorder->addExtraInfo(EX, "Jump taken to #" + convDecToHex(jumpAddressPckg.data[0]));
    jumpIP(jumpAddressPckg.data[0]);
    mssgToDE.sentAt = lastTick;
    fromDEtoMe->sendB(mssgToDE);
    recorder->pushEXtoDEData(mssgToDE);
  }
  else
  {
    recorder->addExtraInfo(EX, "Jump not taken");
    moveIP(instr);
  }
}
