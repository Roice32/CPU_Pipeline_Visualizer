#pragma once

#include "Instruction.h"
#include "Config.h"

class Decode
{
private:
    static byte getExpectedParamCount(byte opCode);
    static bool argumentsMatchExpectedNumber(byte opCode, byte src1, byte src2);
    static bool argumentsMatchExpectedTypes(byte opCode, byte src1, byte src2);
    Instruction decodeInstructionHeader(word header);

public:
    void processFetchWindow(fetch_window newBatch);
};