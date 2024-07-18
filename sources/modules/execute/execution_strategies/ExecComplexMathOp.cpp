#include "ExecComplexMathOp.h"

ExecComplexMathOp::ExecComplexMathOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, refToEX, registers) {};

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
        logComplete(refToEX->getCurrTime(), LoggablePackage(instr, result >> (8 * WORD_BYTES), result));
    }
    else
    {
        word ratio = actualParam1 / actualParam2;
        word modulus = actualParam1 % actualParam2;
        if (ratio == 0 && modulus == 0)
            *regs->flags |= ZERO;
        storeResultAtDest(ratio, R0);
        storeResultAtDest(modulus, R1);
        logComplete(refToEX->getCurrTime(), LoggablePackage(instr, ratio, modulus));
    }
    moveIP(instr);
}


void ExecComplexMathOp::log(LoggablePackage toLog)
{
    printPlainInstruction(toLog.instr);
    printf(" (r0 = %hu, r1 = %hu)", toLog.actualParam1, toLog.actualParam2);
    if (toLog.actualParam1 == 0 && toLog.actualParam2 == 0)
        printf(" Flags.Z=1");
    printf("\n");
}