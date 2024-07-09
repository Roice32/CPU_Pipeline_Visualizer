#include "ExecSimpleMathOp.h"

ExecSimpleMathOp::ExecSimpleMathOp(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers):
    IExecutionStrategy(lsModule, icModule, registers) {};

void ExecSimpleMathOp::executeInstruction(Instruction instr)
{
    word actualParam1 = getFinalArgValue(instr.src1, instr.param1);
    word actualParam2 = getFinalArgValue(instr.src2, instr.param2);
    word result;
    if (instr.opCode == ADD)
        result = actualParam1 + actualParam2;
    else
        result = actualParam1 - actualParam2;
    printf("\t%hu %c %hu = %hu\n", actualParam1, (instr.opCode == ADD ? '+' : '-'), actualParam2, result);
    storeResultAtDest(result, instr.src1, instr.param1);
}