#include "LoadStore.h"
#include "Config.h"

LoadStore::LoadStore(std::shared_ptr<Memory> simulatedMemory,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithIC,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithEX,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
        IMemoryHandler(simulatedMemory), IClockBoundModule(clockSyncVars, 15, "Load/Store"), LSLogger(),
        fromICtoMe(commPipeWithIC), fromEXtoMe(commPipeWithEX) {};

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
    bool EXMadeARequest = fromEXtoMe->pendingA();
    bool ICMadeARequest = fromICtoMe->pendingA();
    if (!EXMadeARequest && !ICMadeARequest)
        return false;

    if (EXMadeARequest)
    {
        SynchronizedDataPackage<MemoryAccessRequest> exReq = fromEXtoMe->getA();
        awaitNextTickToHandle(exReq);
        logAccept(getCurrTime(), exReq.data.reqAddr, true);
        std::vector<word> responseForEX = handleRequestFromEX(exReq.data);
        clock_time lastTick = waitTillLastTick();
        SynchronizedDataPackage<std::vector<word>> syncResponse(responseForEX);
        syncResponse.sentAt = lastTick;
        fromEXtoMe->sendB(syncResponse);
        if (exReq.data.isStoreOperation)
            logComplete(lastTick, LoggablePackage(exReq.data.reqData, exReq.data.reqAddr, true, true));
        else
            logComplete(lastTick, LoggablePackage(responseForEX, exReq.data.reqAddr, false, true));
        return true;
    }
    
    SynchronizedDataPackage<address> lsReq = fromICtoMe->getA();
    awaitNextTickToHandle(lsReq);
    logAccept(getCurrTime(), lsReq.data, false);
    fetch_window responseForIC = bufferedLoadFrom(lsReq.data);
    SynchronizedDataPackage<fetch_window> syncResponse(responseForIC, lsReq.data);
    
    clock_time lastTick = waitTillLastTick();
    syncResponse.sentAt = lastTick;
    fromICtoMe->sendB(syncResponse);
    if (clockSyncVars->running)
        logComplete(lastTick, LoggablePackage(responseForIC, lsReq.data));
    return true;
}