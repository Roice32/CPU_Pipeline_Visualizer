#include "LoadStore.h"
#include "Config.h"

LoadStore::LoadStore(Memory* simulatedMemory, InterThreadCommPipe<address, fetch_window>* commPipeWithIC, InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithEX, register_16b* flagsReg):
    IMemoryHandler(simulatedMemory), requestsFromIC(commPipeWithIC), requestsFromEX(commPipeWithEX), flags(flagsReg) {};

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

word LoadStore::handleRequestFromEX(MemoryAccessRequest req)
{
    if (req.isStoreOperation)
    {
        storeAt(req.reqAddr, req.reqData >> 8);
        storeAt(req.reqAddr + 1, (req.reqData << 8) >> 8);
        return 0;
    }
    else 
    {
        word response = 0;
        response |= loadFrom(req.reqAddr);
        response <<= 8;
        response |= loadFrom(req.reqAddr + 1);
        return response;
    }
}

void LoadStore::run()
{
    word responseForEX;
    address currRequest;
    fetch_window currResponse;
    while (*flags & RUNNING)
    {
        if (requestsFromEX->pendingRequest())
        {
            MemoryAccessRequest exReq = requestsFromEX->getRequest();
            responseForEX = handleRequestFromEX(exReq);
            requestsFromEX->sendResponse(responseForEX);
        }
        // TO DO: When Clock is implemented, turn this into futures
        else if (requestsFromIC->pendingRequest())
        {
            currRequest = requestsFromIC->getRequest();
            currResponse = bufferedLoadFrom(currRequest);
            requestsFromIC->sendResponse(currResponse);
        }
    }
}

LoadStore::~LoadStore()
{
    delete target;
}