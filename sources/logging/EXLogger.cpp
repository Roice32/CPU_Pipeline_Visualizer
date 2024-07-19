#include "EXLogger.h"

EXLogger::EXLogger():
    ILogger("EX") {};

bool EXLogger::canBeReplacedByValue(byte src)
{
    return src != NULL_VAL && src != IMM;
}

void EXLogger::printInstructionWithParamsReplaced(Instruction instr, word actualParam1, word actualParam2)
{
    printf(" (%s", opNames.at((OpCode) instr.opCode));
    if (canBeReplacedByValue(instr.src1) && instr.opCode != MOV)
        printf(" %hu", actualParam1);
    else
        printPlainArg(instr.src1, instr.param1);

    if (instr.src1 * instr.src2 != NULL_VAL)
        printf(",");
    
    if (canBeReplacedByValue(instr.src2))
        printf(" %hu", actualParam2);
    else
        printPlainArg(instr.src2, instr.param2);
    printf(")");
}

void EXLogger::log(LoggablePackage toLog)
{
    printPlainInstruction(toLog.instr);
    if (canBeReplacedByValue(toLog.instr.src1) && toLog.instr.opCode != MOV || canBeReplacedByValue(toLog.instr.src2))
        printInstructionWithParamsReplaced(toLog.instr, toLog.actualParam1, toLog.actualParam2);
    if (toLog.newLine)
        printf("\n");
}

void EXLogger::logDiscard(clock_time timestamp, Instruction instr, address associatedIP, address expectedIP)
{
    char addrInHex[ADDRESS_WIDTH / 4 + 1] = "";
    if (instr.opCode == UNINITIALIZED_MEM || instr.opCode == UNDEFINED)
    {
        printf("[EX@T=%lu]> Ignored malformed / rezidual instruction at #%s\n", timestamp, convDecToHex(associatedIP, addrInHex));
        return;
    }
    printf("[EX@T=%lu]> Ignored instruction '", timestamp);
    printPlainInstruction(instr);
    printf("' from #%s", convDecToHex(associatedIP, addrInHex));
    printf(" (expecting #%s)\n", convDecToHex(expectedIP, addrInHex));
}

void EXLogger::logAccept(clock_time timestamp, Instruction instr, address ip)
{
    char addrInHex[ADDRESS_WIDTH / 4 + 1] = "";
    printf("[EX@T=%lu]> Began executing '", timestamp);
    printPlainInstruction(instr);
    printf("' from #%s\n", convDecToHex(ip, addrInHex));
}

void EXLogger::printFlagsChange(register_16b oldFlagsState, register_16b newFlagsState, bool initSpace)
{
    if (oldFlagsState == newFlagsState)
        return;
    if (initSpace)
        printf(" ");
    if ((oldFlagsState & ZERO) != (newFlagsState & ZERO))
        printf("Flags.Z=%c ", (newFlagsState & ZERO) ? '1' : '0');
    if ((oldFlagsState & EQUAL) != (newFlagsState & EQUAL))
        printf("Flags.E=%c ", (newFlagsState & EQUAL) ? '1' : '0');
    if ((oldFlagsState & GREATER) != (newFlagsState & GREATER))
        printf("Flags.G=%c ", (newFlagsState & GREATER) ? '1' : '0');
}