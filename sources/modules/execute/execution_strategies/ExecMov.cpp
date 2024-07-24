#include "ExecMov.h"

ExecMov::ExecMov(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecMov::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    SynchronizedDataPackage<word> movedValuePckg = getFinalArgValue(instr.src2, instr.param2);

    if (movedValuePckg.exceptionTriggered)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
            movedValuePckg.excpData,
            MISALIGNED_ACCESS_HANDL));
        return;
    }

    SynchronizedDataPackage<word> storeResultPckg = storeResultAtDest(movedValuePckg.data,instr.src1, instr.param1);
    
    if (storeResultPckg.exceptionTriggered)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
            storeResultPckg.excpData,
            MISALIGNED_ACCESS_HANDL));
        return;
    }
    
    clock_time lastTick = refToEX->waitTillLastTick();
    logComplete(lastTick, log(LoggablePackage(instr, 0, movedValuePckg.data))); 
    moveIP(instr);
}

std::string ExecMov::log(LoggablePackage toLog)
{
    std::string result = plainInstructionToString(toLog.instr) + " (" + plainArgToString(toLog.instr.src1, toLog.instr.param1, false);
    result += " = " + std::to_string(toLog.actualParam2) + ")\n";
    return result;
}