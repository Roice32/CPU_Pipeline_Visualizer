#pragma once

#include "LoggablePackage.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>

class ILogger
{
protected:
    const char* moduleName;
    static inline std::unordered_map<OpCode, const char*> opNames;
    static inline std::unordered_map<TypeCode, const char*> typeNames;
    static inline std::shared_ptr<std::ofstream> outputFile;

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

    static std::string fetchWindowToString(fetch_window fw)
    {
        std::string result = "[";
        byte wordsPerFW = FETCH_WINDOW_BYTES / WORD_BYTES;
        char valuesInHex[WORD_BYTES * 2 + 1];
        for (byte ind = 0; ind < wordsPerFW; ++ind)
        {
            result += convDecToHex(fw >> ((wordsPerFW - ind - 1) * 16));
            if (ind != wordsPerFW - 1)
                result += " ";
        }
        result += "]";
        return result;
    }

    static std::string convDecToHex(word source)
    {
        std::string result = "xxxx";
        byte bytesGroup;
        for (byte ind = 0; ind < WORD_BYTES * 2; ++ind)
        {
            bytesGroup = source & 0xf;
            result.at(WORD_BYTES * 2 - ind - 1) = (bytesGroup > 9) ? ('a' + bytesGroup - 10) : ('0' + bytesGroup);
            source >>= 4;
        }
        return result;
    }

    static bool mustDisplayParamValue(byte src)
    {
        return src == IMM || src == ADDR;
    }

    static std::string plainArgToString(byte src, word param, bool spaced = true)
    {
        std::string result = "";
        if (spaced)
            result += " ";
        if (mustDisplayParamValue(src))
        {
            if (src == IMM)
                result += std::to_string(param);
            else
                result += "[" + convDecToHex(param) + "]";
        }
        else
            result += typeNames.at((TypeCode) src);
        return result;
    }

    static std::string plainInstructionToString(Instruction instr)
    {
        std::string result = opNames.at((OpCode) instr.opCode);
        result += plainArgToString(instr.src1, instr.param1);
        if (instr.src1 * instr.src2 != NULL_VAL)
            result += ", ";
        result += plainArgToString(instr.src2, instr.param2);
        return result;
    }

    std::string buildMessageHeader(clock_time timestamp)
    {
        std::string result = "[";
        result += moduleName;
        result += "@T=" + std::to_string(timestamp) + "]> ";
        return result;
    }

    virtual std::string log(LoggablePackage toLog) = 0;

public:
    void logComplete(clock_time timestamp, std::string messageBody)
    {
        std::string message = buildMessageHeader(timestamp) + messageBody;
        if (outputFile->is_open())
            (*outputFile) << message;
        else
            std::cout << message;
    }

    void logAdditional(std::string message)
    {
        if (outputFile->is_open())
            (*outputFile) << message;
        else
            std::cout << message;
    }

    static void openDumpFile(const char* outputFilePath)
    {
        outputFile = std::make_shared<std::ofstream>(outputFilePath);
        assert(outputFile->is_open() && "Could not create specified dump output file");
    }
};