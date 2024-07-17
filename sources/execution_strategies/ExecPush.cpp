#include "ExecPush.h"

ExecPush::ExecPush(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers):
    IExecutionStrategy(commPipeWithLS, registers) {};

void ExecPush::executeInstruction(Instruction instr)
{
    word actualParam = getFinalArgValue(instr.src1, instr.param1);
    log(instr, actualParam);
    assert((*regs->stackPointer > 1) && "Upper limit of the stack exceeded");
    *regs->stackPointer -= WORD_BYTES;
    word newSP = *regs->stackBase + *regs->stackPointer;
    storeDataAt(newSP, 1, std::vector<word> { actualParam });
    moveIP(instr);
}