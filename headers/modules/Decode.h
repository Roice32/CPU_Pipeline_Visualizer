#pragma once

#include "InterThreadCommPipe.h"
#include "Instruction.h"
#include "DecoderCache.h"

#include <memory>

class Decode
{
private:
    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> requestsToIC;
    std::shared_ptr<InterThreadCommPipe<address, Instruction>> requestsFromEX;
    std::shared_ptr<register_16b> const flags;
    DecoderCache cache;

    static byte getExpectedParamCount(byte opCode);
    static bool argumentsMatchExpectedNumber(byte opCode, byte src1, byte src2);
    static bool argumentsMatchExpectedTypes(byte opCode, byte src1, byte src2);
    static bool argumentsAreNotMutuallyExclusive(byte opCode, byte src1, byte src2);
    Instruction decodeInstructionHeader(word header);
    void processFetchWindow(fetch_window newBatch);
    void manageCacheForRequest(address req);

public:
    Decode(std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithIC, std::shared_ptr<InterThreadCommPipe<address, Instruction>> commPipeWithEX, std::shared_ptr<register_16b> const flagsReg);
    void run();
};