#include "Decode.h"

byte Decode::getExpectedParamCount(byte opCode)
{
    if (opCode < 7)
        return 2;
    if (opCode == 13 || opCode == 14)
        return 0;
    return 1;
}

Instruction Decode::decodeInstructionHeader(word instruction)
{
    byte opCode = instruction >> 10;
    if (instruction == 8 || instruction > POP)
        throw "Unknown operation code";
    byte expectedParamCount = getExpectedParamCount(opCode);
    byte src1 = (instruction >> 5) & 0x11111;
    byte src2 = instruction & 0x11111;
    if ((expectedParamCount == 2 && src2 == 0) || (expectedParamCount == 1 && (src1 == 0 && opCode != POP)))
        throw "Wrong arguments";
    // Also check for incompatible arguments
    if (expectedParamCount == 0 || (expectedParamCount == 1 && opCode == POP))
        return Instruction(opCode);
    else if (expectedParamCount == 1)
        return Instruction(opCode, src1);
    return Instruction(opCode, src1, src2);
}

void Decode::processFetchWindow(fetch_window newBatch)
{
    Instruction instr = decodeInstructionHeader(word (newBatch >> 48));
}