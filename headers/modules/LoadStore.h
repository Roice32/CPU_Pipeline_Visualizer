#pragma once

#include "MemoryAccessRequest.h"
#include "InterThreadCommPipe.h"
#include "IMemoryHandler.cpp"

class LoadStore : public IMemoryHandler
{
private:
    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> requestsFromIC;
    std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> requestsFromEX;
    std::shared_ptr<register_16b> flags;

    byte loadFrom(address addr);
    fetch_window bufferedLoadFrom(address addr);
    void storeAt(address addr, byte value);
    word handleRequestFromEX(MemoryAccessRequest req);

public:
    LoadStore(std::shared_ptr<Memory> simulatedMemory, std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithIC, std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithEX, std::shared_ptr<register_16b> flagsReg);
    void run();
};