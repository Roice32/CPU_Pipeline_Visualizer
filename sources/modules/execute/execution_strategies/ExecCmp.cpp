#include "ExecCmp.h"

ExecCmp::ExecCmp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecCmp::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    *regs->flags &= ~(ZERO | EQUAL | GREATER);

    SynchronizedDataPackage<std::vector<word>> actualParam1Pckg = getFinalArgValue(instr.src1, instr.param1);
    SynchronizedDataPackage<std::vector<word>> actualParam2Pckg = getFinalArgValue(instr.src2, instr.param2);
    
    if (actualParam1Pckg.exceptionTriggered || actualParam2Pckg.exceptionTriggered)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP, 
            (actualParam1Pckg.exceptionTriggered ? actualParam1Pckg.excpData : actualParam2Pckg.excpData),
            MISALIGNED_ACCESS_HANDL));
        return;
    }

    if (actualParam1Pckg.data[0] == 0 && actualParam2Pckg.data[0] == 0)
        *regs->flags |= ZERO;
    if (actualParam1Pckg.data[0] == actualParam2Pckg.data[0])
        *regs->flags |= EQUAL;
    if (actualParam1Pckg.data[0] > actualParam2Pckg.data[0])
        *regs->flags |= GREATER;
    moveIP(instr);
    clock_time lastTick = refToEX->waitTillLastTick();
    logComplete(lastTick, log(LoggablePackage(instr, actualParam1Pckg.data[0], actualParam2Pckg.data[0])));
}

std::string ExecCmp::log(LoggablePackage toLog)
{
    std::string result = plainInstructionToString(toLog.instr);
    result += " (" + std::to_string(toLog.actualParam1) + " ? " + std::to_string(toLog.actualParam2) + ")";
    result += printFlagsChange(~*regs->flags, *regs->flags) + "\n";
    return result;
}