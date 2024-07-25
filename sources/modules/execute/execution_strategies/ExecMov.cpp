#include "ExecMov.h"

ExecMov::ExecMov(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecMov::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    SynchronizedDataPackage<std::vector<word>> movedValuePckg = getFinalArgValue(instr.src2, instr.param2, isZReg(instr.src1));

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
    if (isZReg(instr.src1) || isZReg(instr.src2))
        logComplete(lastTick, logComplex(instr, movedValuePckg.data));
    else
        logComplete(lastTick, log(LoggablePackage(instr, 0, movedValuePckg.data[0]))); 
    moveIP(instr);
}

std::string ExecMov::log(LoggablePackage toLog)
{
    std::string result = "Finished executing: " + plainInstructionToString(toLog.instr) + " (" + plainArgToString(toLog.instr.src1, toLog.instr.param1, false);
    result += " = " + std::to_string(toLog.actualParam2) + ")\n";
    return result;
}

std::string ExecMov::logComplex(Instruction instr, std::vector<word> movedVal)
{
    std::string result = "Finished executing: " + plainInstructionToString(instr) + " (" + plainArgToString(instr.src1, instr.param1, false) + " =";
    for (byte wordInd = 0; wordInd < WORDS_PER_Z_REGISTER; ++wordInd)
        result += " " + convDecToHex(movedVal[wordInd]);
    result += ")\n";
    return result;
}