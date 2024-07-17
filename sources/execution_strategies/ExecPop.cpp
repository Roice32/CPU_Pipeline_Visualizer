#include "ExecPop.h"

ExecPop::ExecPop(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, std::vector<word>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers):
    IExecutionStrategy(commPipeWithLS, registers) {};

void ExecPop::executeInstruction(Instruction instr)
{
    log(instr);
    assert((*regs->stackSize - *regs->stackPointer > 1) && "Lower limit of the stack exceeded");

    if (instr.src1 != NULL_VAL)
    {
        word topOfStack = *regs->stackBase + *regs->stackPointer;
        word valueOnTop = requestDataAt(topOfStack, 1)[0];
        storeResultAtDest(valueOnTop, instr.src1, instr.param1);
    }
    *regs->stackPointer += WORD_BYTES;
    moveIP(instr);
}