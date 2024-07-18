#include "ExecSimpleMathOp.h"

ExecSimpleMathOp::ExecSimpleMathOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, refToEX, registers) {};

void ExecSimpleMathOp::executeInstruction(Instruction instr)
{
    word actualParam1 = getFinalArgValue(instr.src1, instr.param1);
    word actualParam2 = getFinalArgValue(instr.src2, instr.param2);
    word result;
    if (instr.opCode == ADD)
        result = actualParam1 + actualParam2;
    else
        result = actualParam1 - actualParam2;
    storeResultAtDest(result, instr.src1, instr.param1);
    if (result == 0)
        *regs->flags |= ZERO;
    logComplete(refToEX->getCurrTime(),LoggablePackage { EXLogPackage(instr, actualParam1, result) });
    moveIP(instr);
}

void ExecSimpleMathOp::log(LoggablePackage toLog)
{
    printPlainInstruction(toLog.ex.instr);
    printf(" (");
    printPlainArg(toLog.ex.instr.src1, toLog.ex.instr.param1, false);
    printf(" = %hu)", toLog.ex.actualParam2);
    if (toLog.ex.actualParam2 == 0)
        printf(" Flags.Z=1");
    printf("\n");
}