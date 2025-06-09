#include "LoadStore.h"
#include "Config.h"

LoadStore::LoadStore(std::shared_ptr<Memory> simulatedMemory,
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithIC,
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithEX,
  std::shared_ptr<ClockSyncPackage> clockSyncVars,
  std::shared_ptr<ExecutionRecorder> recorder):
    IMemoryHandler(simulatedMemory),
    IClockBoundModule(clockSyncVars, LS_CYCLES_PER_OP),
    fromICtoMe(commPipeWithIC),
    fromEXtoMe(commPipeWithEX),
    cache(KWayAssociativeCache<word>()),
    recorder(recorder) {};

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
  recorder->modifyModuleState(LS,
                              "Handling request from EX for "
                                + std::string(req.isStoreOperation ? "store at #" : "load from #")
                                + convDecToHex(req.reqAddr) + " with "
                                + std::to_string(req.wordsSizeOfReq) + " words.");
  physicalMemoryAccessHappened = false;
  address currAddr;

  if (req.isStoreOperation)
  {
    for (byte ind = 0; ind < req.wordsSizeOfReq; ++ind)
    {
      currAddr = req.reqAddr + WORD_BYTES * ind;
      cache.prepareForOps(currAddr);
      DiscardedCacheElement<word> removedElem = cache.store(req.reqData[ind], getCurrTime());
      recorder->storeLSCacheLine(cache.getCurrReqTag(),
                                 cache.getCurrReqIndex(),
                                 cache.getCurrReqInnerIndex(),
                                 req.reqData[ind],
                                 getCurrTime());
      if (removedElem.discardHappened)
      {
        storeAt(removedElem.addr, removedElem.data >> 8);
        storeAt(removedElem.addr + 1, removedElem.data);
        recorder->recordMemoryChanges({removedElem.addr}, {removedElem.data});
        physicalMemoryAccessHappened = true;
      }
      else
        recorder->addExtraInfo(LS, "Cache hit for word at #" + convDecToHex(currAddr));

      SynchronizedDataPackage<fetch_window> fwInvalidationPckg;
      fwInvalidationPckg.data = currAddr;
      fwInvalidationPckg.exceptionTriggered = true;
      fromICtoMe->sendB(fwInvalidationPckg);
      recorder->pushLStoICData(fwInvalidationPckg);
    }
    return std::vector<word> {};
  }

  std::vector<word> response;
  word currWord;
  for (byte ind = 0; ind < req.wordsSizeOfReq; ++ind)
  {
    currAddr = req.reqAddr + WORD_BYTES * ind;
    cache.prepareForOps(currAddr);
    if (cache.isAHit())
    {
      recorder->addExtraInfo(LS, "Cache hit for word at #" + convDecToHex(currAddr));
      currWord = cache.get(getCurrTime());
      response.push_back(currWord);
      continue;
    }

    currWord = 0;
    currWord |= loadFrom(currAddr);
    currWord <<= 8;
    currWord |= loadFrom(currAddr + 1);
    response.push_back(currWord);
    
    DiscardedCacheElement<word> removedElem = cache.store(currWord, getCurrTime(), true);
    recorder->storeLSCacheLine(cache.getCurrReqTag(),
                               cache.getCurrReqIndex(),
                               cache.getCurrReqInnerIndex(),
                               currWord,
                               getCurrTime());
    if (removedElem.discardHappened)
    {
      storeAt(removedElem.addr, removedElem.data >> 8);
      storeAt(removedElem.addr + 1, removedElem.data);
      recorder->recordMemoryChanges({removedElem.addr}, {removedElem.data});
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
    recorder->popPipeData(EXtoLS);
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
      responseForEX = handleRequestFromEX(exReq.data);
      syncResponse.data = responseForEX;
    }

    if (!physicalMemoryAccessHappened)
      shortenThisCycleBy(LS_CYCLES_PER_OP - LS_CYCLES_PER_OP_WITH_CACHE_HIT);
    
    clock_time lastTick = waitTillLastTick();
    syncResponse.sentAt = lastTick;
    fromEXtoMe->sendB(syncResponse);
    
    recorder->pushLStoEXData(syncResponse);
    recorder->modifyModuleState(LS,
                                "Finished handling request from EX from cycle "
                                + std::to_string(exReq.sentAt)
                                + " regarding #" + convDecToHex(exReq.data.reqAddr)
                                + " with "
                                + std::string(physicalMemoryAccessHappened ? "" : "no")
                                + " physical memory access.");
    return;
  }
  
  SynchronizedDataPackage<address> lsReq = fromICtoMe->getA();
  recorder->popPipeData(Pipes::ICtoLS);
  awaitNextTickToHandle(lsReq);
  if (clockSyncVars->running)
    recorder->modifyModuleState(LS,
                                "Handling request from IC for fetch window at #" + convDecToHex(lsReq.data));

  fetch_window responseForIC = bufferedLoadFrom(lsReq.data);
  fetch_window updatedResponseForIC = 0;
  for (byte wordInd = 0; wordInd < FETCH_WINDOW_WORDS; ++wordInd)
  {
    updatedResponseForIC <<= WORD_BYTES * 8;
    cache.prepareForOps(lsReq.data + wordInd * WORD_BYTES);
    if (cache.isAHit())
      updatedResponseForIC |= cache.get(getCurrTime());
    else
      updatedResponseForIC |= (responseForIC >> ((FETCH_WINDOW_WORDS - 1 - wordInd) * WORD_BYTES * 8)) & 0xffff;
  }
  SynchronizedDataPackage<fetch_window> syncResponse(updatedResponseForIC, lsReq.data);
  
  clock_time lastTick = waitTillLastTick();
  syncResponse.sentAt = lastTick;
  fromICtoMe->sendB(syncResponse);
  recorder->pushLStoICData(syncResponse);
  recorder->modifyModuleState(LS,
                              "Finished handling request from IC from cycle "
                              + std::to_string(lsReq.sentAt)
                              + " regarding #" + convDecToHex(lsReq.data));
  return;
}
