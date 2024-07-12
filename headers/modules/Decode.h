#pragma once

#include "Execute.h"
#include "Config.h"

class Decode
{
private:
    InterThreadCommPipe<address, fetch_window>* requestsToIC;
    register_16b* const IP;
    Execute* EXModule;

    static byte getExpectedParamCount(byte opCode);
    static bool argumentsMatchExpectedNumber(byte opCode, byte src1, byte src2);
    static bool argumentsMatchExpectedTypes(byte opCode, byte src1, byte src2);
    static bool argumentsAreNotMutuallyExclusive(byte opCode, byte src1, byte src2);
    Instruction decodeInstructionHeader(word header);
    void moveIP(byte const paramsCount);

public:
    Decode(InterThreadCommPipe<address, fetch_window>* commPipeWithIC, register_16b* const IP);
    void processFetchWindow(fetch_window newBatch);
    void setEXModule(Execute* exModuleRef);
    void printReceived(word newBatch);
    void run();
};