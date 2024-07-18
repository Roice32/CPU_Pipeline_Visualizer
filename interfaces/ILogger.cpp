#pragma once

#include "LoggablePackage.h"

#include <cstdio>
#include <unordered_map>

class ILogger
{
protected:
    const char* moduleName;
    static inline std::unordered_map<OpCode, const char*> opNames;
    static inline std::unordered_map<TypeCode, const char*> typeNames;

    ILogger(const char* moduleName): moduleName(moduleName)
    {
        if (opNames.empty())
        {
            opNames.insert({ADD, "add"});
            opNames.insert({SUB, "sub"});
            opNames.insert({MOV, "mov"});
            opNames.insert({MUL, "mul"});
            opNames.insert({DIV, "div"});
            opNames.insert({CMP, "cmp"});
            opNames.insert({JMP, "jmp"});
            opNames.insert({JE, "je"});
            opNames.insert({JL, "jl"});
            opNames.insert({JG, "jg"});
            opNames.insert({JZ, "jz"});
            opNames.insert({CALL, "call"});
            opNames.insert({RET, "ret"});
            opNames.insert({END_SIM, "end_sim"});
            opNames.insert({PUSH, "push"});
            opNames.insert({POP, "pop"});
        }
        if (typeNames.empty())
        {
            typeNames.insert({NULL_VAL, ""});
            typeNames.insert({SP_REG, "sp"});
            typeNames.insert({ST_BASE, "stack_base"});
            typeNames.insert({ST_SIZE, "stack_size"});
            typeNames.insert({R0, "r0"});
            typeNames.insert({R1, "r1"});
            typeNames.insert({R2, "r2"});
            typeNames.insert({R3, "r3"});
            typeNames.insert({R4, "r4"});
            typeNames.insert({R5, "r5"});
            typeNames.insert({R6, "r6"});
            typeNames.insert({R7, "r7"});
            typeNames.insert({ADDR_R0, "[r0]"});
            typeNames.insert({ADDR_R1, "[r1]"});
            typeNames.insert({ADDR_R2, "[r2]"});
            typeNames.insert({ADDR_R3, "[r3]"});
            typeNames.insert({ADDR_R4, "[r4]"});
            typeNames.insert({ADDR_R5, "[r5]"});
            typeNames.insert({ADDR_R6, "[r6]"});
            typeNames.insert({ADDR_R7, "[r7]"});
            // If registers' count ever changes, this needs changing, too.
        }
    };

    static char* convDecToHex(word source, char* dest)
    {
        byte bytesGroup;
        for (byte ind = 0; ind < WORD_BYTES * 2; ++ind)
        {
            bytesGroup = source & 0xf;
            dest[WORD_BYTES * 2 - ind - 1] = (bytesGroup > 9) ? ('a' + bytesGroup - 10) : ('0' + bytesGroup);
            source >>= 4;
        }
        return dest;
    }

    static bool mustDisplayParamValue(byte src)
    {
        return src == IMM || src == ADDR;
    }

    static void printPlainArg(byte src, word param, bool spaced = true)
    {
        if (spaced)
            printf(" ");
        if (mustDisplayParamValue(src))
        {
            if (src == IMM)
                printf("%hu", param);
            else
                printf("[%hu]", param);
        }
        else
            printf("%s", typeNames.at((TypeCode) src));
    }

    static void printPlainInstruction(Instruction instr)
    {
        printf("%s", opNames.at((OpCode) instr.opCode));
        printPlainArg(instr.src1, instr.param1);
        if (instr.src1 * instr.src2 != NULL_VAL)
            printf(",");
        printPlainArg(instr.src2, instr.param2);
    }

    virtual void log(LoggablePackage toLog) = 0;

public:
    void logComplete(clock_time timestamp, LoggablePackage toLog)
    {
        printf("[%s@T=%lu]>", moduleName, timestamp);
        log(toLog);
    }
};