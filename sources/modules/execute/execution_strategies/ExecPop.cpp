#include "ExecPop.h"

ExecPop::ExecPop(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, refToEX, registers) {};

std::string ExecPop::log(LoggablePackage toLog)
{
    std::string result = plainInstructionToString(toLog.instr);
    if (toLog.instr.src1 != NULL_VAL)
    {
        result += "(" + plainArgToString(toLog.instr.src1, toLog.instr.param1);
        result += " = " + std::to_string(toLog.actualParam1) + ")";
    }
    result += "\n";
    return result;
}

void ExecPop::executeInstruction(Instruction instr)
{
    assert((*regs->stackSize - *regs->stackPointer >= WORD_BYTES) && "Lower limit of the stack exceeded");

    word valueOnTop;
    if (instr.src1 != NULL_VAL)
    {
        word topOfStack = *regs->stackBase + *regs->stackPointer;
        valueOnTop = requestDataAt(topOfStack, 1)[0];
        storeResultAtDest(valueOnTop, instr.src1, instr.param1);
    }
    *regs->stackPointer += WORD_BYTES;
    moveIP(instr);
    clock_time lastTick = refToEX->waitTillLastTick();
    logComplete(lastTick, log(LoggablePackage(instr, valueOnTop)));
}