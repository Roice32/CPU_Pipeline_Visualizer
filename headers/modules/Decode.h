#pragma once

#include "InterThreadCommPipe.h"
#include "Instruction.h"
#include "Config.h"

#include <memory>

class Decode
{
private:
    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> requestsToIC;
    std::shared_ptr<InterThreadCommPipe<byte, Instruction>> requestsFromEX;
    std::shared_ptr<register_16b> const IP;

    static byte getExpectedParamCount(byte opCode);
    static bool argumentsMatchExpectedNumber(byte opCode, byte src1, byte src2);
    static bool argumentsMatchExpectedTypes(byte opCode, byte src1, byte src2);
    static bool argumentsAreNotMutuallyExclusive(byte opCode, byte src1, byte src2);
    Instruction decodeInstructionHeader(word header);
    void moveIP(byte const paramsCount); // TO DO: Get rid of this

public:
    Decode(std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithIC, std::shared_ptr<InterThreadCommPipe<byte, Instruction>> commPipeWithEX, std::shared_ptr<register_16b> const IP);
    void processFetchWindow(fetch_window newBatch);
    void run();
};