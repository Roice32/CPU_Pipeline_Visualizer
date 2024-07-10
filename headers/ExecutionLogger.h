#pragma once

#include "Instruction.h"

#include <cstdio>
#include <unordered_map>

class ExecutionLogger
{
private:
    static inline std::unordered_map<OpCode, const char*> opNames;
    static inline std::unordered_map<TypeCode, const char*> typeNames;

    static bool mustDisplayParamValue(byte src);
    static bool canBeReplacedByValue(byte src);
    static void printPlainArg(byte src, word param);
    static void printPlainInstruction(Instruction instr);
    static void printInstructionWithParamsReplaced(Instruction instr, word actualParam1 = 0, word actualParam2 = 0);

protected:
    ExecutionLogger();
    static void log(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true);
};