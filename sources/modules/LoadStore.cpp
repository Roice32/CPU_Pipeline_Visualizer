#include "LoadStore.h"
#include "Config.h"

LoadStore::LoadStore(std::shared_ptr<Memory> simulatedMemory,
    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithIC,
    std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithEX,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
        IMemoryHandler(simulatedMemory), IClockBoundModule(clockSyncVars, 15, "Load/Store"),
        requestsFromIC(commPipeWithIC), requestsFromEX(commPipeWithEX) {};

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

bool LoadStore::executeModuleLogic()
{
    bool EXMadeARequest = requestsFromEX->pendingRequest();
    bool ICMadeARequest = requestsFromIC->pendingRequest();
    if (!EXMadeARequest && !ICMadeARequest)
        return false;

    word responseForEX;
    address currRequest;
    fetch_window currResponse;
    
    if (EXMadeARequest)
    {
        MemoryAccessRequest exReq = requestsFromEX->getRequest();
        responseForEX = handleRequestFromEX(exReq);
        waitTillLastTick();
        requestsFromEX->sendResponse(responseForEX);
        return true;
    }
    
    currRequest = requestsFromIC->getRequest();
    currResponse = bufferedLoadFrom(currRequest);
    waitTillLastTick();
    requestsFromIC->sendResponse(currResponse);
    return true;
}