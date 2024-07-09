#include "ExecPop.h"

ExecPop::ExecPop(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers):
    IExecutionStrategy(lsModule, icModule, registers) {};

void ExecPop::executeInstruction(Instruction instr)
{
    if (regs->stackSize - regs->stackPointer < 2)
        throw "Lower limit of the stack exceeded";
    if (instr.src1 != NULL_VAL)
    {
        word topOfStack = regs->stackBase + regs->stackPointer;
        word valueOnTop = requestDataAt(topOfStack);
        storeResultAtDest(valueOnTop, instr.src1, instr.param1);
    }
    regs->stackPointer += 2;
}