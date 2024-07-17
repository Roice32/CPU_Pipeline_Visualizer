#include "ExecCmp.h"

ExecCmp::ExecCmp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers):
    IExecutionStrategy(commPipeWithLS, registers) {};

void ExecCmp::executeInstruction(Instruction instr)
{
    *regs->flags &= ~(ZERO | EQUAL | GREATER);
    word actualParam1 = getFinalArgValue(instr.src1, instr.param1);
    word actualParam2 = getFinalArgValue(instr.src2, instr.param2);
    if (actualParam1 == 0 && actualParam2 == 0)
        *regs->flags |= ZERO;
    if (actualParam1 == actualParam2)
        *regs->flags |= EQUAL;
    if (actualParam1 > actualParam2)
        *regs->flags |= GREATER;
    log(instr, actualParam1, actualParam2);
    moveIP(instr);
}

void ExecCmp::log(Instruction instr, word actualParam1, word actualParam2, bool newLine)
{
    printf(">");
    printPlainInstruction(instr);
    printf(" (%hu ? %hu)", actualParam1, actualParam2);
    printFlagsChange(~*regs->flags, *regs->flags);
    printf("\n");
}