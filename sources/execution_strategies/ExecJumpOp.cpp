#include "ExecJumpOp.h"

ExecJumpOp::ExecJumpOp(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers):
    IExecutionStrategy(lsModule, icModule, registers) {};

void ExecJumpOp::executeInstruction(Instruction instr)
{
    word jumpAddress = getFinalArgValue(instr.src1, instr.param1);
    log(instr, jumpAddress, 0, false);

    bool plainJump = (instr.opCode == JMP);
    bool equalJump = (instr.opCode == JE && (regs->flags & EQUAL));
    bool lessJump = (instr.opCode == JL && !(regs->flags & EQUAL) && !(regs->flags & GREATER));
    bool greaterJump = (instr.opCode == JG && (regs->flags & GREATER));
    bool zeroJump = (instr.opCode == JZ && (regs->flags & ZERO));

    if (plainJump || equalJump || lessJump || greaterJump || zeroJump)
    {
        regs->IP = jumpAddress;
        printf(" (yes)\n");
    }
    else
        printf(" (no)\n");
}

ExecJumpOp::~ExecJumpOp() {};