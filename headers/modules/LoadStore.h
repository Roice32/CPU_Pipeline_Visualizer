#pragma once

#include "InterThreadCommPipe.h"
#include "IMemoryHandler.cpp"

class LoadStore : public IMemoryHandler
{
private:
    InterThreadCommPipe<address, fetch_window>* requestsFromIC;
    register_16b* flags;

public:
    LoadStore(Memory* simulatedMemory, InterThreadCommPipe<address, fetch_window>* commPipeWithIC, register_16b* flagsReg);
    byte loadFrom(address addr);
    fetch_window bufferedLoadFrom(address addr);
    void storeAt(address addr, byte value);
    void run();
    ~LoadStore();
};