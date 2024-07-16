#pragma once

#include "ClockSyncPackage.h"
#include "MemoryAccessRequest.h"
#include "InterThreadCommPipe.h"
#include "IMemoryHandler.cpp"
#include "IClockBoundModule.cpp"

class LoadStore : public IMemoryHandler, public IClockBoundModule
{
private:
    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> requestsFromIC;
    std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> requestsFromEX;

    byte loadFrom(address addr) override;
    fetch_window bufferedLoadFrom(address addr) override;
    void storeAt(address addr, byte value) override;
    word handleRequestFromEX(MemoryAccessRequest req);

public:
    LoadStore(std::shared_ptr<Memory> simulatedMemory,
        std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithIC,
        std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithEX,
        std::shared_ptr<ClockSyncPackage> clockSyncVars);

    bool executeModuleLogic() override;
};