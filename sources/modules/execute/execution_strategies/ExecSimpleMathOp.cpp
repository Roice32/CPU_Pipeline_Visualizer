#include "ExecSimpleMathOp.h"

ExecSimpleMathOp::ExecSimpleMathOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecSimpleMathOp::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    SynchronizedDataPackage<std::vector<word>> actualParam1Pckg = getFinalArgValue(instr.src1, instr.param1);
    SynchronizedDataPackage<std::vector<word>> actualParam2Pckg = getFinalArgValue(instr.src2, instr.param2);

    if (actualParam1Pckg.exceptionTriggered || actualParam2Pckg.exceptionTriggered)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
            (actualParam1Pckg.exceptionTriggered ? actualParam1Pckg.excpData : actualParam2Pckg.excpData),
            MISALIGNED_ACCESS_HANDL));
        return;
    }

    std::vector<word> result;
    if (instr.src1 >= Z0 && instr.src1 <= Z3)
    {
        result.push_back( instr.opCode == ADD ?
            actualParam1Pckg.data[0] + actualParam2Pckg.data[0] :
            actualParam1Pckg.data[0] - actualParam2Pckg.data[0]);
        if (result[0] == 0)
            *regs->flags |= ZERO;
    }
    else
        for (byte wordInd = 0; wordInd < WORDS_PER_Z_REGISTER; ++wordInd)
        {
            result.push_back(instr.opCode == ADD ?
                actualParam1Pckg.data[wordInd] + actualParam2Pckg.data[wordInd] :
                actualParam1Pckg.data[wordInd] - actualParam2Pckg.data[wordInd]);
            if (result[wordInd] == 0)
                *regs->flags |= ZERO;
        }
    storeResultAtDest(result, instr.src1, instr.param1);
    moveIP(instr);
    clock_time lastTick = refToEX->waitTillLastTick();
    if (instr.src1 >= Z0 && instr.src1 <= Z3)
        logComplete(lastTick, logComplex(instr));
    else
        logComplete(lastTick, log(LoggablePackage(instr, actualParam1Pckg.data[0], result[0])));
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

std::string ExecSimpleMathOp::logComplex(Instruction instr)
{
    std::string result = opNames.at((OpCode) instr.opCode);
    result += " ";
    result += typeNames.at((TypeCode) instr.src1);
    result += ", ";
    result += typeNames.at((TypeCode) instr.src2);
    result += "(";
    result += typeNames.at((TypeCode) instr.src1);
    result += "=";
    for (byte wordInd = 0; wordInd < WORDS_PER_Z_REGISTER; ++wordInd)
        result += " " + convDecToHex((*regs->zRegisters[instr.src1 - Z0])[wordInd]);
    result += ")";
    for (byte wordInt = 0; wordInt < WORDS_PER_Z_REGISTER; ++wordInt)
        if ((*regs->zRegisters[instr.src1 - Z0])[wordInt] == 0)
        {
            result += " Flags.Z=1";
            break;
        }
    result += "\n";
    return result;
}