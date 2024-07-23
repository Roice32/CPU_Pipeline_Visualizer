#pragma once

#include "DELogger.h"
#include "InterThreadCommPipe.h"
#include "Instruction.h"
#include "DecoderCache.h"
#include "SynchronizedDataPackage.h"
#include "IClockBoundModule.cpp"

#include <memory>

class Decode: public IClockBoundModule, public DELogger
{
private:
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, address>> fromICtoMe;
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> fromMetoEX;
    std::shared_ptr<register_16b> const flags;
    DecoderCache cache;
    address discardUntilAddr;

    static byte getExpectedParamCount(byte opCode);
    static bool argumentsMatchExpectedNumber(byte opCode, byte src1, byte src2);
    static bool argumentsMatchExpectedTypes(byte opCode, byte src1, byte src2);
    static bool argumentsAreNotMutuallyExclusive(byte opCode, byte src1, byte src2);
    Instruction decodeInstructionHeader(word header);
    bool processFetchWindow(fetch_window newBatch);

public:
    Decode(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, address>> commPipeWithIC,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithEX,
        std::shared_ptr<ClockSyncPackage> clockSyncVars,
        std::shared_ptr<register_16b> flags);

    bool executeModuleLogic() override;
};