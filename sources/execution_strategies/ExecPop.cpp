#include "ExecPop.h"

ExecPop::ExecPop(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, refToEX, registers) {};

void ExecPop::executeInstruction(Instruction instr)
{
    logComplete(refToEX->getCurrTime(),LoggablePackage { EXLogPackage(instr) });
    assert((*regs->stackSize - *regs->stackPointer >= WORD_BYTES) && "Lower limit of the stack exceeded");

    if (instr.src1 != NULL_VAL)
    {
        word topOfStack = *regs->stackBase + *regs->stackPointer;
        word valueOnTop = requestDataAt(topOfStack, 1)[0];
        storeResultAtDest(valueOnTop, instr.src1, instr.param1);
    }
    *regs->stackPointer += WORD_BYTES;
    moveIP(instr);
}