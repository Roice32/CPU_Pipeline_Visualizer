#include "ExecPush.h"

ExecPush::ExecPush(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecPush::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    SynchronizedDataPackage<word> actualParamPckg = getFinalArgValue(instr.src1, instr.param1);

    if (actualParamPckg.exceptionTriggered)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
            actualParamPckg.excpData,
            MISALIGNED_ACCESS_HANDL));
        return;
    }

    assert((*regs->stackPointer >= WORD_BYTES) && "Upper limit of the stack exceeded");
    *regs->stackPointer -= WORD_BYTES;
    word newSP = *regs->stackBase + *regs->stackPointer;

    SynchronizedDataPackage<std::vector<word>> storeResultPckg = storeDataAt(newSP, 1, std::vector<word> { actualParamPckg.data });

    if (storeResultPckg.exceptionTriggered)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
            storeResultPckg.excpData,
            MISALIGNED_ACCESS_HANDL));
        return;
    }

    moveIP(instr);
    clock_time lastTick = refToEX->waitTillLastTick();
    logComplete(lastTick, log(LoggablePackage(instr, actualParamPckg.data)));
}