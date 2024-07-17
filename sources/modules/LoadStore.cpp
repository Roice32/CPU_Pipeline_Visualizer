#include "LoadStore.h"
#include "Config.h"

LoadStore::LoadStore(std::shared_ptr<Memory> simulatedMemory,
    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithIC,
    std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, std::vector<word>>> commPipeWithEX,
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

std::vector<word> LoadStore::handleRequestFromEX(MemoryAccessRequest req)
{
    if (req.isStoreOperation)
    {
        for (byte ind = 0; ind < req.wordsSizeOfReq; ++ind)
        {
            storeAt(req.reqAddr + WORD_BYTES * ind, req.reqData[ind] >> 8);
            storeAt(req.reqAddr + WORD_BYTES * ind + 1, req.reqData[ind]);
        }
        return std::vector<word> {};
    }
    else 
    {
        std::vector<word> response;
        for (byte ind = 0; ind < req.wordsSizeOfReq; ++ind)
        {
            response.push_back(0);
            response[ind] |= loadFrom(req.reqAddr + WORD_BYTES * ind);
            response[ind] <<= 8;
            response[ind] |= loadFrom(req.reqAddr + WORD_BYTES * ind + 1);
        }
        return response;
    }
}

bool LoadStore::executeModuleLogic()
{
    bool EXMadeARequest = requestsFromEX->pendingRequest();
    bool ICMadeARequest = requestsFromIC->pendingRequest();
    if (!EXMadeARequest && !ICMadeARequest)
        return false;

    std::vector<word> responseForEX;
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