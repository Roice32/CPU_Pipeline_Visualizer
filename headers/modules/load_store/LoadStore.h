#pragma once

#include "ClockSyncPackage.h"
#include "LSLogger.h"
#include "MemoryAccessRequest.h"
#include "InterThreadCommPipe.h"
#include "SynchronizedDataPackage.h"
#include "IMemoryHandler.cpp"
#include "IClockBoundModule.cpp"

class LoadStore : public IMemoryHandler, public IClockBoundModule, public LSLogger
{
private:
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> fromICtoMe;
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> fromEXtoMe;

    byte loadFrom(address addr) override;
    fetch_window bufferedLoadFrom(address addr) override;
    void storeAt(address addr, byte value) override;
    std::vector<word> handleRequestFromEX(MemoryAccessRequest req);

public:
    LoadStore(std::shared_ptr<Memory> simulatedMemory,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithIC,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithEX,
        std::shared_ptr<ClockSyncPackage> clockSyncVars);

    bool executeModuleLogic() override;
};