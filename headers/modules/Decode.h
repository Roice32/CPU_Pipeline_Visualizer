#pragma once

#include "Execute.h"
#include "Config.h"

class Decode
{
private:
    register_16b* const IP;
    Execute* EXModule;

    static byte getExpectedParamCount(byte opCode);
    static bool argumentsMatchExpectedNumber(byte opCode, byte src1, byte src2);
    static bool argumentsMatchExpectedTypes(byte opCode, byte src1, byte src2);
    static bool argumentsAreNotMutuallyExclusive(byte opCode, byte src1, byte src2);
    Instruction decodeInstructionHeader(word header);
    void moveIP(byte const paramsCount);

public:
    Decode(register_16b* const IP): IP(IP) {};
    void processFetchWindow(fetch_window newBatch);
    void setEXModule(Execute* exModuleRef);
    void printReceived(word newBatch);
};