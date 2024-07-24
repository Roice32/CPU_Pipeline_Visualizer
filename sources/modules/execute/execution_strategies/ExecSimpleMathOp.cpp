#include "ExecSimpleMathOp.h"

ExecSimpleMathOp::ExecSimpleMathOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecSimpleMathOp::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    SynchronizedDataPackage<word> actualParam1Pckg = getFinalArgValue(instr.src1, instr.param1);
    SynchronizedDataPackage<word> actualParam2Pckg = getFinalArgValue(instr.src2, instr.param2);

    if (actualParam1Pckg.exceptionTriggered || actualParam2Pckg.exceptionTriggered)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
            (actualParam1Pckg.exceptionTriggered ? actualParam1Pckg.excpData : actualParam2Pckg.excpData),
            MISALIGNED_ACCESS_HANDL));
            return;
    }

    word result;
    if (instr.opCode == ADD)
        result = actualParam1Pckg.data + actualParam2Pckg.data;
    else
        result = actualParam1Pckg.data - actualParam2Pckg.data;
    storeResultAtDest(result, instr.src1, instr.param1);
    if (result == 0)
        *regs->flags |= ZERO;
    moveIP(instr);
    clock_time lastTick = refToEX->waitTillLastTick();
    logComplete(lastTick, log(LoggablePackage(instr, actualParam1Pckg.data, result)));
}

std::string ExecSimpleMathOp::log(LoggablePackage toLog)
{
    std::string result = plainInstructionToString(toLog.instr) + " (" + plainArgToString(toLog.instr.src1, toLog.instr.param1, false);
    result += " = " + std::to_string(toLog.actualParam2) + ")";
    if (toLog.actualParam2 == 0)
        result += " Flags.Z=1";
    result += "\n";
    return result;
}