#pragma once

#include "Instruction.h"
#include "Config.h"

class Decode
{
private:
    byte getExpectedParamCount(byte opCode);
    Instruction decodeInstructionHeader(word header);

public:
    void processFetchWindow(fetch_window newBatch);
};