#include "ExecPush.h"

ExecPush::ExecPush(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers):
    IExecutionStrategy(lsModule, icModule, registers) {};

void ExecPush::executeInstruction(Instruction instr)
{
    if (regs->stackPointer < 2)
        throw "Upper limit of the stack exceeded";
    word actualParam = getFinalArgValue(instr.src1, instr.param1);
    regs->stackPointer -= 2;
    word newSP = regs->stackBase + regs->stackPointer;
    storeDataAt(newSP, actualParam);
}