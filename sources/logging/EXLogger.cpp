#include "EXLogger.h"

EXLogger::EXLogger():
    ILogger("EX") {};

bool EXLogger::canBeReplacedByValue(byte src)
{
    return src != NULL_VAL && src != IMM;
}

std::string EXLogger::printInstructionWithParamsReplaced(Instruction instr, word actualParam1, word actualParam2)
{
    std::string result = "(";
    result += opNames.at((OpCode) instr.opCode);
    if (canBeReplacedByValue(instr.src1) && instr.opCode != MOV)
        result += " " + std::to_string(actualParam1);
    else
        result += plainArgToString(instr.src1, instr.param1);

    if (instr.src1 * instr.src2 != NULL_VAL)
        result += ",";
    
    if (canBeReplacedByValue(instr.src2))
        result += " " + std::to_string(actualParam2);
    else
        result += plainArgToString(instr.src2, instr.param2);
    result += ")";
    return result;
}

std::string EXLogger::log(LoggablePackage toLog)
{
    std::string result = plainInstructionToString(toLog.instr);
    if (canBeReplacedByValue(toLog.instr.src1) && toLog.instr.opCode != MOV || canBeReplacedByValue(toLog.instr.src2))
        result += printInstructionWithParamsReplaced(toLog.instr, toLog.actualParam1, toLog.actualParam2);
    if (toLog.newLine)
        result += "\n";
    return result;
}

std::string EXLogger::logDiscard(Instruction instr, address associatedIP, address expectedIP)
{
    std::string result = "";
    if (instr.opCode == UNINITIALIZED_MEM || instr.opCode == UNDEFINED)
    {
        result = "Ignored malformed / residual instruction at #" + convDecToHex(associatedIP) + "\n";
        return result;
    }
    result = "Ignored instruction '" + plainInstructionToString(instr) + "' from #" + convDecToHex(associatedIP) + " (expecting #" + convDecToHex(expectedIP) + ")\n";
    return result;
}

std::string EXLogger::logAccept(Instruction instr, address ip)
{
    std::string result = "Began executing '" + plainInstructionToString(instr) + "' from #" + convDecToHex(ip) + "\n";
    return result;
}

std::string EXLogger::printFlagsChange(register_16b oldFlagsState, register_16b newFlagsState, bool initSpace)
{
    std::string result = "";
    if (oldFlagsState == newFlagsState)
        return result;
    if (initSpace)
        result += " ";
    if ((oldFlagsState & ZERO) != (newFlagsState & ZERO))
    {
        result += "Flags.Z=";
        result += newFlagsState & ZERO ? "1" : "0";
        result += " ";
    }
    if ((oldFlagsState & EQUAL) != (newFlagsState & EQUAL))
    {
        result += "Flags.E=";
        result += newFlagsState & EQUAL ? "1" : "0";
        result += " ";
    }
    if ((oldFlagsState & GREATER) != (newFlagsState & GREATER))
    {
        result += "Flags.G=";
        result += newFlagsState & GREATER ? "1" : "0";
        result += " ";
    }
    return result;
}