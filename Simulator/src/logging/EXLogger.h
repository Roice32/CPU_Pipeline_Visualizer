#pragma once

#include "ILogger.cpp"

class EXLogger: public ILogger
{
protected:
    EXLogger();
    static bool canBeReplacedByValue(byte src);
    static std::string printInstructionWithParamsReplaced(Instruction instr, word actualParam1 = 0, word actualParam2 = 0);
    static std::string printFlagsChange(register_16b oldFlagsState, register_16b newFlagsState, bool initSpace = true);
    virtual std::string log(LoggablePackage toLog) override;

public:
    static std::string logDiscard(Instruction instr, address associatedIP, address expectedIP = 0);
    static std::string logAccept(Instruction instr, address ip);
    static std::string logSkip(address emptyAddress, address newAwaitedIP);
};