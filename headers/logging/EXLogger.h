#pragma once

#include "ILogger.cpp"

#include <cstdio>
#include <unordered_map>

class EXLogger: public ILogger
{
protected:
    static inline std::unordered_map<OpCode, const char*> opNames;
    static inline std::unordered_map<TypeCode, const char*> typeNames;

    EXLogger();
    static bool mustDisplayParamValue(byte src);
    static bool canBeReplacedByValue(byte src);
    static void printPlainArg(byte src, word param, bool spaced = true);
    static void printPlainInstruction(Instruction instr);
    static void printInstructionWithParamsReplaced(Instruction instr, word actualParam1 = 0, word actualParam2 = 0);
    static void printFlagsChange(register_16b oldFlagsState, register_16b newFlagsState, bool initSpace = true);
    virtual void log(LoggablePackage toLog) override;
};