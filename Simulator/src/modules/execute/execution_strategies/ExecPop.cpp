#include "ExecPop.h"

ExecPop::ExecPop(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

std::string ExecPop::log(LoggablePackage toLog)
{
    std::string result = "Finished executing: " + plainInstructionToString(toLog.instr);
    if (toLog.instr.src1 != NULL_VAL)
    {
        result += "(" + plainArgToString(toLog.instr.src1, toLog.instr.param1);
        result += " = " + std::to_string(toLog.actualParam1) + ")";
    }
    result += "\n";
    return result;
}

void ExecPop::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;

    if (*regs->stackSize < *regs->stackPointer || *regs->stackSize - *regs->stackPointer < WORD_BYTES)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
            POP_OVERFLOW,
            STACK_OVERFLOW_HANDL));
        return;
    }

    SynchronizedDataPackage<std::vector<word>> valueOnTopPckg;
    if (instr.src1 != NULL_VAL)
    {
        word topOfStack = *regs->stackBase + *regs->stackPointer;
        valueOnTopPckg = requestDataAt(topOfStack, 1);

        if (valueOnTopPckg.exceptionTriggered)
        {
            handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
                valueOnTopPckg.excpData, MISALIGNED_ACCESS_HANDL));
            return;
        }

        SynchronizedDataPackage<word> storeResultPckg = storeResultAtDest(valueOnTopPckg.data,instr.src1, instr.param1);

        if (storeResultPckg.exceptionTriggered)
        {
            handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
                storeResultPckg.excpData,
                MISALIGNED_ACCESS_HANDL));
            return;
        }
    }
    *regs->stackPointer += WORD_BYTES;
    moveIP(instr);
    clock_time lastTick = refToEX->waitTillLastTick();
    logComplete(lastTick, log(LoggablePackage(instr, valueOnTopPckg.data.size() > 0 ? valueOnTopPckg.data[0] : 0)));
}