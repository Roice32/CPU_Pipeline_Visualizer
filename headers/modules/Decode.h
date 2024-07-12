#pragma once

#include "InterThreadCommPipe.h"
#include "Instruction.h"
#include "Config.h"

class Decode
{
private:
    InterThreadCommPipe<address, fetch_window>* requestsToIC;
    InterThreadCommPipe<byte, Instruction>* requestsFromEX;
    register_16b* const IP;

    static byte getExpectedParamCount(byte opCode);
    static bool argumentsMatchExpectedNumber(byte opCode, byte src1, byte src2);
    static bool argumentsMatchExpectedTypes(byte opCode, byte src1, byte src2);
    static bool argumentsAreNotMutuallyExclusive(byte opCode, byte src1, byte src2);
    Instruction decodeInstructionHeader(word header);
    void moveIP(byte const paramsCount); // TO DO: Get rid of this

public:
    Decode(InterThreadCommPipe<address, fetch_window>* commPipeWithIC, InterThreadCommPipe<byte, Instruction>* commPipeWithEX, register_16b* const IP);
    void processFetchWindow(fetch_window newBatch);
    void run();
};