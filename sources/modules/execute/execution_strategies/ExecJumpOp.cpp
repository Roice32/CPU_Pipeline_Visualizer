#include "ExecJumpOp.h"

ExecJumpOp::ExecJumpOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, refToEX, registers),
        fromDEtoMe(commPipeWithDE) {};

void ExecJumpOp::executeInstruction(Instruction instr)
{
    word jumpAddress = getFinalArgValue(instr.src1, instr.param1);

    bool plainJump = (instr.opCode == JMP);
    bool equalJump = (instr.opCode == JE && (*regs->flags & EQUAL));
    bool lessJump = (instr.opCode == JL && !(*regs->flags & EQUAL) && !(*regs->flags & GREATER));
    bool greaterJump = (instr.opCode == JG && (*regs->flags & GREATER));
    bool zeroJump = (instr.opCode == JZ && (*regs->flags & ZERO));

    logComplete(refToEX->getCurrTime(), LoggablePackage(instr, jumpAddress, 0, false));
    if (plainJump || equalJump || lessJump || greaterJump || zeroJump)
    {
        printf(" (yes)\n");
        *regs->IP = jumpAddress;
        fromDEtoMe->sendB(jumpAddress);
    }
    else
    {
        printf(" (no)\n");
        moveIP(instr);
    }
}