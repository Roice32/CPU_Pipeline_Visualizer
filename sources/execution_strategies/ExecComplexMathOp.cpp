#include "ExecComplexMathOp.h"

ExecComplexMathOp::ExecComplexMathOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers):
    IExecutionStrategy(commPipeWithLS, registers) {};

void ExecComplexMathOp::executeInstruction(Instruction instr)
{
    word actualParam1 = getFinalArgValue(instr.src1, instr.param1);
    word actualParam2 = getFinalArgValue(instr.src2, instr.param2);
    if (instr.opCode == MUL)
    {
        unsigned int result = ((unsigned int) actualParam1) * actualParam2;
        if (result == 0)
            *regs->flags |= ZERO;

        storeResultAtDest(result >> (8 * WORD_BYTES), R0);
        storeResultAtDest(result, R1);
        log(instr, result >> (8 * WORD_BYTES), result);
    }
    else
    {
        word ratio = actualParam1 / actualParam2;
        word modulus = actualParam1 % actualParam2;
        if (ratio == 0 && modulus == 0)
            *regs->flags |= ZERO;
        storeResultAtDest(ratio, R0);
        storeResultAtDest(modulus, R1);
        log(instr, ratio, modulus);
    }
    moveIP(instr);
}


void ExecComplexMathOp::log(Instruction instr, word r0Result, word r1Result, bool newLine)
{
    printf(">");
    printPlainInstruction(instr);
    printf(" (r0 = %hu, r1 = %hu)", r0Result, r1Result);
    if (r0Result == 0 && r1Result == 0)
        printf(" Flags.Z=1");
    printf("\n");
}