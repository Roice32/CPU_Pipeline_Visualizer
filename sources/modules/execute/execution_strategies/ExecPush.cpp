#include "ExecPush.h"

ExecPush::ExecPush(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, refToEX, registers) {};

void ExecPush::executeInstruction(Instruction instr)
{
    word actualParam = getFinalArgValue(instr.src1, instr.param1);
    assert((*regs->stackPointer >= WORD_BYTES) && "Upper limit of the stack exceeded");
    *regs->stackPointer -= WORD_BYTES;
    word newSP = *regs->stackBase + *regs->stackPointer;
    storeDataAt(newSP, 1, std::vector<word> { actualParam });
    moveIP(instr);
    clock_time lastTick = refToEX->waitTillLastTick();
    logComplete(lastTick, log(LoggablePackage(instr, actualParam)));
}