#include "LoadStore.h"
#include "Config.h"

LoadStore::LoadStore(std::shared_ptr<Memory> simulatedMemory,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithIC,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithEX,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
        IMemoryHandler(simulatedMemory), IClockBoundModule(clockSyncVars, 15), LSLogger(),
        fromICtoMe(commPipeWithIC), fromEXtoMe(commPipeWithEX), cache(KWayAssociativeCache<word>()) {};

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
    physicalMemoryAccessHappened = false;
    if (req.isStoreOperation)
    {
        for (byte ind = 0; ind < req.wordsSizeOfReq; ++ind)
        {
            cache.prepareForOps(req.reqAddr + WORD_BYTES * ind);
            DiscardedCacheElement<word> removedElem = cache.store(req.reqData[ind], getCurrTime());
            if (removedElem.discardHappened)
            {
                storeAt(removedElem.addr, removedElem.data >> 8);
                storeAt(removedElem.addr + 1, removedElem.data);
                physicalMemoryAccessHappened = true;
            }
        }
        return std::vector<word> {};
    }

    std::vector<word> response;
    word currWord;
    for (byte ind = 0; ind < req.wordsSizeOfReq; ++ind)
    {
        cache.prepareForOps(req.reqAddr + WORD_BYTES * ind);
        if (cache.isAHit())
        {
            currWord = cache.get(getCurrTime());
            response.push_back(currWord);
            continue;
        }

        currWord = 0;
        currWord |= loadFrom(req.reqAddr + WORD_BYTES * ind);
        currWord <<= 8;
        currWord |= loadFrom(req.reqAddr + WORD_BYTES * ind + 1);
        response.push_back(currWord);
        
        DiscardedCacheElement<word> removedElem = cache.store(currWord, getCurrTime());
        if (removedElem.discardHappened)
        {
            storeAt(removedElem.addr, removedElem.data >> 8);
            storeAt(removedElem.addr + 1, removedElem.data);
            logComplete(getCurrTime(), "Swapped word at #" +
                convDecToHex(removedElem.addr) + 
                " from cache with word at #" +
                convDecToHex(req.reqAddr) +
                " from physical memory.\n");
        }

        physicalMemoryAccessHappened = true;
    }
    return response;
}

void LoadStore::executeModuleLogic()
{
    bool EXMadeARequest = fromEXtoMe->pendingA();
    bool ICMadeARequest = fromICtoMe->pendingA();
    if (!EXMadeARequest && !ICMadeARequest)
        return;

    bool EXShouldHavePriority = true;
    if (EXMadeARequest && ICMadeARequest)
        if (fromEXtoMe->peekA().sentAt == getCurrTime())
            EXShouldHavePriority = false;

    if (EXMadeARequest && EXShouldHavePriority)
    {
        SynchronizedDataPackage<MemoryAccessRequest> exReq = fromEXtoMe->getA();
        awaitNextTickToHandle(exReq);
        std::vector<word> responseForEX;
        SynchronizedDataPackage<std::vector<word>> syncResponse{};
        if (exReq.data.reqAddr % 2 == 1)
        {
            syncResponse.exceptionTriggered = true;
            syncResponse.excpData = exReq.data.reqAddr;
            syncResponse.handlerAddr = MISALIGNED_ACCESS_HANDL;
        }
        else
        {
            logComplete(getCurrTime(), logAccept(exReq.data.reqAddr, true));
            responseForEX = handleRequestFromEX(exReq.data);
            syncResponse.data = responseForEX;
        }

        if (!physicalMemoryAccessHappened)
            startTimeOfCurrOp -= 8;
        
        clock_time lastTick = waitTillLastTick();
        syncResponse.sentAt = lastTick;
        fromEXtoMe->sendB(syncResponse);
        if (exReq.data.isStoreOperation && !syncResponse.exceptionTriggered)
            logComplete(lastTick, log(LoggablePackage(exReq.data.reqData, exReq.data.reqAddr, true, true)));
        else
            logComplete(lastTick, log(LoggablePackage(responseForEX, exReq.data.reqAddr, false, true)));
        if (!physicalMemoryAccessHappened)
            logAdditional("\t(Entirely using LS's cache)\n");
        return;
    }
    
    SynchronizedDataPackage<address> lsReq = fromICtoMe->getA();
    awaitNextTickToHandle(lsReq);
    if (clockSyncVars->running)
        logComplete(getCurrTime(), logAccept(lsReq.data, false));
    fetch_window responseForIC = bufferedLoadFrom(lsReq.data);
    SynchronizedDataPackage<fetch_window> syncResponse(responseForIC, lsReq.data);
    
    clock_time lastTick = waitTillLastTick();
    syncResponse.sentAt = lastTick;
    fromICtoMe->sendB(syncResponse);
    if (clockSyncVars->running)
        logComplete(lastTick, log(LoggablePackage(responseForIC, lsReq.data)));
    return;
}