#include "ExecuteADD.h"

void ExecuteADD::executeInstruction(Instruction instr)
{
    word actualParam1 = getFinalArgValue(instr.src1, instr.param1);
    word actualParam2 = getFinalArgValue(instr.src2, instr.param2);
    word result = actualParam1 + actualParam2;
    storeResultAtDest(result, instr.src1, instr.param1);
    printf("Did %hu + %hu = %hu and stored the result at %hu", actualParam1, actualParam2, result, instr.src1);
}