#include "ExecComplexMathOp.h"

ExecComplexMathOp::ExecComplexMathOp(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers):
    IExecutionStrategy(lsModule, icModule, registers) {};

void ExecComplexMathOp::executeInstruction(Instruction instr)
{
    word actualParam1 = getFinalArgValue(instr.src1, instr.param1);
    word actualParam2 = getFinalArgValue(instr.src2, instr.param2);
    if (instr.opCode == MUL)
    {
        unsigned int result = ((unsigned int) actualParam1) * actualParam2;
        if (result == 0)
            regs->flags |= ZERO;
        printf("\t%hu * %hu = %u (FLAG.Z = %hu)\n", actualParam1, actualParam2, result, regs->flags);
        storeResultAtDest(result >> 16, R0);
        storeResultAtDest((result << 16) >> 16, R1);
    }
    else
    {
        word ratio = actualParam1 / actualParam2;
        word modulus = actualParam1 % actualParam2;
        if (ratio == 0 && modulus == 0)
            regs->flags |= ZERO;
        printf("\t%hu / %hu = %hu mod %hu\n", actualParam1, actualParam2, ratio, modulus);
        storeResultAtDest(ratio, R0);
        storeResultAtDest(modulus, R1);
    }
}