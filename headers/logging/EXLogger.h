#pragma once

#include "ILogger.cpp"

#include <cstdio>

class EXLogger: public ILogger
{
protected:
    EXLogger();
    static bool canBeReplacedByValue(byte src);
    static void printInstructionWithParamsReplaced(Instruction instr, word actualParam1 = 0, word actualParam2 = 0);
    static void printFlagsChange(register_16b oldFlagsState, register_16b newFlagsState, bool initSpace = true);
    virtual void log(LoggablePackage toLog) override;

public:
    static void logDiscard(clock_time timestamp, Instruction instr, address associatedIP, address expectedIP = 0);
    static void logAccept(clock_time timestamp, Instruction instr, address ip);
};