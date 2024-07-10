#include "ExecMov.h"

ExecMov::ExecMov(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers):
    IExecutionStrategy(lsModule, icModule, registers) {};

void ExecMov::executeInstruction(Instruction instr)
{
    word movedValue = getFinalArgValue(instr.src2, instr.param2);
    log(instr, 0, movedValue); 
    storeResultAtDest(movedValue, instr.src1, instr.param1);
}