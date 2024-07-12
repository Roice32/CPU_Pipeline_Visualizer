#include "LoadStore.h"
#include "Config.h"

LoadStore::LoadStore(Memory* simulatedMemory, InterThreadCommPipe<address, fetch_window>* commPipeWithIC, register_16b* flagsReg):
    IMemoryHandler(simulatedMemory)
{
    requestsFromIC = commPipeWithIC;
    flags = flagsReg;
}

byte LoadStore::loadFrom(address addr)
{
    return target->getMemoryCell(addr);
}

fetch_window LoadStore::bufferedLoadFrom(address addr)
{
    fetch_window bufferedFetchResult = 0;
    for (byte currByte = 0; currByte < FETCH_WINDOW_BYTES; ++currByte)
    {
        byte readByte = target->getMemoryCell(addr + currByte);
        bufferedFetchResult <<= 8;
        bufferedFetchResult |= readByte;
    }
    return bufferedFetchResult;
}

void LoadStore::storeAt(address addr, byte value)
{
    target->setMemoryCell(addr, value);
}

void LoadStore::run()
{
    address currRequest;
    fetch_window currResponse;
    while (*flags & RUNNING)
    {
        // TO DO: When Clock is implemented, turn this into futures
        if (!requestsFromIC->pendingRequest())
            continue;
        currRequest = requestsFromIC->getRequest();
        currResponse = bufferedLoadFrom(currRequest);
        requestsFromIC->sendResponse(currResponse);
    }
}

LoadStore::~LoadStore()
{
    delete target;
}