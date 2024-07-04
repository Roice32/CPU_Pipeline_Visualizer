#include "Decode.h"

byte Decode::getExpectedParamCount(byte opCode)
{
    if (opCode < 7)
        return 2;
    if (opCode == 13 || opCode == 14)
        return 0;
    return 1;
}

bool Decode::argumentsMatchExpectedNumber(byte opCode, byte src1, byte src2)
{
    byte expectedParamCount = getExpectedParamCount(opCode);
    if (expectedParamCount == 2 && src2 == 0)
        return false;
    if (expectedParamCount == 1 && src1 == 0 && opCode != POP)
        return false;
    return true;
}

bool Decode::argumentsMatchExpectedTypes(byte opCode, byte src1, byte src2)
{
    if((opCode == ADD || opCode == SUB) && src1 == IMM)
        return false;
    if(opCode == DIV && src2 == NULL)
        return false;
    // WIP
    return true;
}

Instruction Decode::decodeInstructionHeader(word instruction)
{
    byte opCode = instruction >> 10;
    if (instruction == 8 || instruction > POP)
        throw "Unknown operation code";
    byte src1 = (instruction >> 5) & 0x11111;
    byte src2 = instruction & 0x11111;
    
    if (!argumentsMatchExpectedNumber(opCode, src1, src2))
        throw "Wrong arguments count";
    
    if (!argumentsMatchExpectedTypes(opCode, src1, src2))
        throw "Wrong arguments types";
    
    // Also check for incompatible arguments

    return Instruction(opCode, src1, src2);
}

void Decode::processFetchWindow(fetch_window newBatch)
{
    Instruction instr = decodeInstructionHeader(word (newBatch >> 48));
}