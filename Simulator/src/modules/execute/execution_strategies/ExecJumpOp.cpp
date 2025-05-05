#include "ExecJumpOp.h"

ExecJumpOp::ExecJumpOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
  IClockBoundModule* refToEX,
  std::shared_ptr<CPURegisters> registers):
    IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecJumpOp::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  Instruction instr = instrPackage.data;
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
  logComplete(lastTick, log(LoggablePackage(instr, jumpAddressPckg.data[0], 0, false)));
  if (plainJump || equalJump || lessJump || greaterJump || zeroJump)
  {
    logAdditional(" (yes)\n");
    *regs->IP = jumpAddressPckg.data[0];
    mssgToDE.sentAt = lastTick;
    fromDEtoMe->sendB(mssgToDE);
  }
  else
  {
    logAdditional(" (no)\n");
    moveIP(instr);
  }
}

std::string ExecJumpOp::log(LoggablePackage toLog)
{
  std::string result = "Finished executing: ";
  result += opNames.at((OpCode) toLog.instr.opCode);
  result += " #" + convDecToHex(toLog.actualParam1);
  return result;
}