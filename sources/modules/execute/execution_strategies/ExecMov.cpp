#include "ExecMov.h"

ExecMov::ExecMov(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecMov::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    SynchronizedDataPackage<std::vector<word>> movedValuePckg = getFinalArgValue(instr.src2, instr.param2);

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
    if (instr.src1 >= Z0 && instr.src1 <= Z3 ||
        instr.src2 >= Z0 && instr.src2 <= Z3)
        logComplete(lastTick, logComplex(instr, movedValuePckg.data));
    else
        logComplete(lastTick, log(LoggablePackage(instr, 0, movedValuePckg.data[0]))); 
    moveIP(instr);
}

std::string ExecMov::log(LoggablePackage toLog)
{
    std::string result = plainInstructionToString(toLog.instr) + " (" + plainArgToString(toLog.instr.src1, toLog.instr.param1, false);
    result += " = " + std::to_string(toLog.actualParam2) + ")\n";
    return result;
}

std::string ExecMov::logComplex(Instruction instr, std::vector<word> movedVal)
{
    std::string result = plainInstructionToString(instr) + "(" + plainArgToString(instr.src1, instr.param1, false) + " =";
    for (byte wordInd = 0; wordInd < WORDS_PER_Z_REGISTER; ++wordInd)
        result += " " + convDecToHex(movedVal[wordInd]);
    result += ")";
    for (byte wordInd = 0; wordInd < WORDS_PER_Z_REGISTER; ++wordInd)
        if (movedVal[wordInd] == 0)
        {
            result += " Flags.Z=1";
            break;
        }
    result += "\n";
    return result;
}