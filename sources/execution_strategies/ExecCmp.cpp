#include "ExecCmp.h"

ExecCmp::ExecCmp(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers):
    IExecutionStrategy(lsModule, icModule, registers) {};

void ExecCmp::executeInstruction(Instruction instr)
{
    regs->flags &= ~(ZERO | EQUAL | GREATER);
    word actualParam1 = getFinalArgValue(instr.src1, instr.param1);
    word actualParam2 = getFinalArgValue(instr.src2, instr.param2);
    log(instr, actualParam1, actualParam2);
    if (actualParam1 == 0 && actualParam2 == 0)
        regs->flags |= ZERO;
    if (actualParam1 == actualParam2)
        regs->flags |= EQUAL;
    if (actualParam1 > actualParam2)
        regs->flags |= GREATER;
}