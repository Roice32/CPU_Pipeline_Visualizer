#include "InstructionCache.h"

InstructionCache::InstructionCache(
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, SynchronizedDataPackage<address>>> commPipeWithDE,
    std::shared_ptr<ClockSyncPackage> clockSyncVars,
    std::shared_ptr<register_16b> ip,
    std::shared_ptr<ExecutionRecorder> recorder
  ):
  IClockBoundModule(clockSyncVars, IC_CYCLES_PER_OP),
  fromMetoLS(commPipeWithLS), fromMetoDE(commPipeWithDE), internalIP(SIM_START_ADDR), cache(), recorder(recorder) {};

fetch_window InstructionCache::getFetchWindowFromLS(address addr) {
  clock_time reqSendTime = getCurrTime();
  SynchronizedDataPackage<address> syncReq(addr, clockSyncVars->cycleCount);
  syncReq.sentAt = reqSendTime;
  fromMetoLS->sendA(syncReq);

  if (clockSyncVars->running)
    recorder->modifyModuleState(IC, "Awaiting fetch window at #" + convDecToHex(addr) + " from LS");
    recorder->pushICtoLSData(syncReq);

  enterIdlingState();
  SynchronizedDataPackage<fetch_window> receivedPckg;
  while (clockSyncVars->running)
  {
    while (!fromMetoLS->pendingB() && clockSyncVars->running)
      returnFromIdlingState();
    
    if (!clockSyncVars->running)
      return 0;
    receivedPckg = fromMetoLS->getB();
    recorder->popPipeData(Pipes::LStoIC);
    recorder->modifyModuleState(IC, "Received fetch window "
                                    + fwToStr(receivedPckg.data)
                                    + " from LS at #"
                                    + convDecToHex(receivedPckg.associatedIP));
    
    if (!receivedPckg.exceptionTriggered)
      break;
    
    address invalidatedFW = receivedPckg.data / FETCH_WINDOW_BYTES * FETCH_WINDOW_BYTES;
    cache.prepareForOps(invalidatedFW);
    if (cache.invalidate())
      recorder->invalidateICCacheLine(cache.getCurrReqIndex());
  }
  
  awaitNextTickToHandle(receivedPckg);
  return receivedPckg.data;
}

bool InstructionCache::checkIPChangeSignal()
{
  if (!fromMetoDE->pendingB())
    return false;
  SynchronizedDataPackage<address> signalFromDE = fromMetoDE->getB();
  recorder->popPipeData(Pipes::DEtoIC);
  clock_time timeReceived = getCurrTime();
  internalIP = signalFromDE.data / FETCH_WINDOW_BYTES * FETCH_WINDOW_BYTES;
  recorder->modifyModuleState(IC, "Received IP change signal from DE to #"
                                  + convDecToHex(signalFromDE.data)
                                  + " (aligned as #"
                                  + convDecToHex(internalIP)
                                  + ")");
  recorder->modifICInternalIP(internalIP);
  return true;
}

void InstructionCache::executeModuleLogic()
{
  if (fromMetoLS->pendingB())
  {
    address invalidatedFW = fromMetoLS->getB().data / FETCH_WINDOW_BYTES * FETCH_WINDOW_BYTES;
    recorder->popPipeData(Pipes::LStoIC);
    cache.prepareForOps(invalidatedFW);
    if(cache.invalidate())
      recorder->invalidateICCacheLine(cache.getCurrReqIndex());
  }

  fetch_window currBatch;
  cache.prepareForOps(internalIP);
  bool fwAlreadyInCache = cache.isAHit();
  if (fwAlreadyInCache)
  {
    shortenThisCycleBy(IC_CYCLES_PER_OP - IC_CYCLES_PER_OP_WITH_CACHE_HIT);
    currBatch = cache.get();
  }
  else
  {
    currBatch = getFetchWindowFromLS(internalIP);
    address removedElement = cache.store(currBatch);
    recorder->swapICCacheLine(cache.getCurrReqLine(), cache.getCurrReqIndex());
  }
  SynchronizedDataPackage<fetch_window> syncResponse(currBatch, internalIP);
  internalIP += FETCH_WINDOW_BYTES;
  recorder->modifICInternalIP(internalIP);
  
  clock_time lastTick = waitTillLastTick();
  syncResponse.sentAt = lastTick;
  if (clockSyncVars->running && !checkIPChangeSignal())
  {
    fromMetoDE->sendA(syncResponse);
    recorder->pushICtoDEData(syncResponse);
    if (fwAlreadyInCache)
      recorder->addExtraInfo(IC, "Cache hit for fetch window at #" + convDecToHex(syncResponse.associatedIP));
  }
}

void InstructionCache::run()
{
  recorder->modifyModuleState(IC, "Starting simulation from #" + convDecToHex(SIM_START_ADDR));
  clockSyncVars->ICReady = true;
  startCurrOpTimer();
  executeModuleLogic();
  IClockBoundModule::run();
}