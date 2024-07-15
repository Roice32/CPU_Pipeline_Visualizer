#include "ExecPop.h"

ExecPop::ExecPop(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS, std::shared_ptr<CPURegisters> registers):
    IExecutionStrategy(commPipeWithLS, registers) {};

void ExecPop::executeInstruction(Instruction instr)
{
    log(instr);
    executeInstructionNoLog(instr);
}

void ExecPop::executeInstructionNoLog(Instruction instr)
{
    assert((*regs->stackSize - *regs->stackPointer > 1) && "Lower limit of the stack exceeded");

    if (instr.src1 != NULL_VAL)
    {
        word topOfStack = *regs->stackBase + *regs->stackPointer;
        word valueOnTop = requestDataAt(topOfStack);
        storeResultAtDest(valueOnTop, instr.src1, instr.param1);
    }
    *regs->stackPointer += 2;
}