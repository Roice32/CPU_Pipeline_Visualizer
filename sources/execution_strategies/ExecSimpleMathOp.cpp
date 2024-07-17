#include "ExecSimpleMathOp.h"

ExecSimpleMathOp::ExecSimpleMathOp(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, std::vector<word>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers):
    IExecutionStrategy(commPipeWithLS, registers) {};

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
    log(instr, actualParam1, result);
    moveIP(instr);
}

void ExecSimpleMathOp::log(Instruction instr, word actualParam1, word result, bool newLine)
{
    printf(">");
    printPlainInstruction(instr);
    printf(" (");
    printPlainArg(instr.src1, instr.param1, false);
    printf(" = %hu)", result);
    if (result == 0)
        printf(" Flags.Z=1");
    printf("\n");
}