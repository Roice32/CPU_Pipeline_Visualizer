#pragma once

#include "MemoryAccessRequest.h"
#include "InterThreadCommPipe.h"
#include "IMemoryHandler.cpp"

class LoadStore : public IMemoryHandler
{
private:
    InterThreadCommPipe<address, fetch_window>* requestsFromIC;
    InterThreadCommPipe<MemoryAccessRequest, word>* requestsFromEX;
    register_16b* flags;

public:
    LoadStore(Memory* simulatedMemory, InterThreadCommPipe<address, fetch_window>* commPipeWithIC, InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithEX, register_16b* flagsReg);
    byte loadFrom(address addr);
    fetch_window bufferedLoadFrom(address addr);
    void storeAt(address addr, byte value);
    word handleRequestFromEX(MemoryAccessRequest req);
    void run();
    ~LoadStore();
};