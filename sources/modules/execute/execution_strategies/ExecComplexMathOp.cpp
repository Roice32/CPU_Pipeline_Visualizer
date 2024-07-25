#include "ExecComplexMathOp.h"

ExecComplexMathOp::ExecComplexMathOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

bool ExecComplexMathOp::handleNormalComplexOp(Instruction instr, word actualParam1, word actualParam2)
{
    if (instr.opCode == MUL)
    {
        uint32_t result = ((uint32_t) actualParam1) * actualParam2;
        if (result == 0)
            *regs->flags |= ZERO;

        storeResultAtDest(std::vector<word> { word (result >> (8 * WORD_BYTES))}, R0 );
        storeResultAtDest(std::vector<word> { word (result) }, R1);
        clock_time lastTick = refToEX->waitTillLastTick();
        logComplete(lastTick, log(LoggablePackage(instr, result >> (8 * WORD_BYTES), result)));
    }
    else
    {
        if(actualParam2 == 0)
        {
            handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
                DIV_BY_ZERO,
                DIV_BY_ZERO_HANDL));
            return true;
        }

        word ratio = actualParam1 / actualParam2;
        word modulus = actualParam1 % actualParam2;
        if (ratio == 0 && modulus == 0)
            *regs->flags |= ZERO;
        storeResultAtDest(std::vector<word> { ratio }, R0);
        storeResultAtDest(std::vector<word> { modulus }, R1);
        clock_time lastTick = refToEX->waitTillLastTick();
        logComplete(lastTick, log(LoggablePackage(instr, ratio, modulus)));
    }
    return false;
}

bool ExecComplexMathOp::handleZRegComplexOp(Instruction instr, std::vector<word> actualParam1, std::vector<word> actualParam2)
{
    uint32_t product;
    word ratio, modulus;
    std::vector<word> zHigh, zLow;
    for (byte wordInd = 0; wordInd < Z_REGISTER_COUNT; ++wordInd)
    {
        if (instr.opCode == MUL)
        {
            product = ((uint32_t) actualParam1[wordInd]) * actualParam2[wordInd];
            zHigh.push_back(product >> (WORD_BYTES * 8));
            zLow.push_back(product);
            if (product == 0)
                *regs->flags |= ZERO;
        }
        else
        {
            if (actualParam2[wordInd] == 0)
            {
                handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
                    DIV_BY_ZERO,
                    DIV_BY_ZERO_HANDL));
                return true;
            }
            ratio = actualParam1[wordInd] / actualParam2[wordInd];
            modulus = actualParam1[wordInd] % actualParam2[wordInd];
            zHigh.push_back(ratio);
            zLow.push_back(modulus);
            if (ratio == 0 && modulus == 0)
                *regs->flags |= ZERO;
        }
    }
    clock_time lastTick = refToEX->waitTillLastTick();
    storeResultAtDest(zHigh, Z0);
    storeResultAtDest(zLow, Z1);
    logComplete(lastTick, logComplex(instr, zHigh, zLow));
    return false;
}

void ExecComplexMathOp::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    bool zRegInvolved = isZReg(instr.src1) || isZReg(instr.src2);
    SynchronizedDataPackage<std::vector<word>> actualParam1Pckg = getFinalArgValue(instr.src1, instr.param1, zRegInvolved);
    SynchronizedDataPackage<std::vector<word>> actualParam2Pckg = getFinalArgValue(instr.src2, instr.param2, zRegInvolved);

    if (actualParam1Pckg.exceptionTriggered || actualParam2Pckg.exceptionTriggered)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
            (actualParam1Pckg.exceptionTriggered ? actualParam1Pckg.excpData : actualParam2Pckg.excpData),
            MISALIGNED_ACCESS_HANDL));
        return;
    }

    bool exceptionOccuredDuringOp;
    if (zRegInvolved)
        exceptionOccuredDuringOp = handleZRegComplexOp(instr, actualParam1Pckg.data, actualParam2Pckg.data);
    else
        exceptionOccuredDuringOp = handleNormalComplexOp(instr, actualParam1Pckg.data[0], actualParam2Pckg.data[0]);
    
    if (!exceptionOccuredDuringOp)
        moveIP(instr);
}

std::string ExecComplexMathOp::log(LoggablePackage toLog)
{
    std::string result = "Finished executing: " + plainInstructionToString(toLog.instr) + " (r0 = ";
    result += std::to_string(toLog.actualParam1) + ", r1 = " + std::to_string(toLog.actualParam2) + ")";
    if (toLog.actualParam1 == 0 && toLog.actualParam2 == 0)
        result += " Flags.Z=1";
    result += "\n";
    return result;
}

std::string ExecComplexMathOp::logComplex(Instruction instr, std::vector<word> highResult, std::vector<word> lowResult)
{
    std::string result = "Finished executing: " + plainInstructionToString(instr);
    result += " (z0 =";
    for (byte wordInd = 0; wordInd < WORDS_PER_Z_REGISTER; ++wordInd)
        result += " " + convDecToHex(highResult[wordInd]);
    result += ", z1 =";
    for (byte wordInd = 0; wordInd < WORDS_PER_Z_REGISTER; ++wordInd)
        result += " " + convDecToHex(lowResult[wordInd]);
    result += ")";
    for (byte wordInt = 0; wordInt < WORDS_PER_Z_REGISTER; ++wordInt)
        if (highResult[wordInt] == 0 && lowResult[wordInt] == 0)
        {
            result += " Flags.Z=1";
            break;
        }
    result += "\n";
    return result;
}