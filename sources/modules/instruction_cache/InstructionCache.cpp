#include "InstructionCache.h"

InstructionCache::InstructionCache(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, SynchronizedDataPackage<address>>> commPipeWithDE,
    std::shared_ptr<ClockSyncPackage> clockSyncVars,
    std::shared_ptr<register_16b> ip):
        IClockBoundModule(clockSyncVars, 3),
        fromMetoLS(commPipeWithLS), fromMetoDE(commPipeWithDE), internalIP(0xfff0), cache() {};

fetch_window InstructionCache::getFetchWindowFromLS(address addr) {
    clock_time reqSendTime = getCurrTime();
    SynchronizedDataPackage<address> syncReq(addr, clockSyncVars->cycleCount);
    syncReq.sentAt = reqSendTime;
    fromMetoLS->sendA(syncReq);
    if (clockSyncVars->running)
        logComplete(reqSendTime, logRequest(internalIP));
    enterIdlingState();
    while (!fromMetoLS->pendingB() && clockSyncVars->running)
        returnFromIdlingState();

    if (!clockSyncVars->running)
        return 0;

    SynchronizedDataPackage<fetch_window> receivedPckg = fromMetoLS->getB();
    awaitNextTickToHandle(receivedPckg);
    return receivedPckg.data;
}

void InstructionCache::executeModuleLogic()
{
    if (fromMetoDE->pendingB())
    {
        SynchronizedDataPackage<address> signalFromDE = fromMetoDE->getB();
        awaitNextTickToHandle(signalFromDE);
        internalIP = signalFromDE.data / FETCH_WINDOW_BYTES * FETCH_WINDOW_BYTES;
        logComplete(getCurrTime(), logJump(signalFromDE.data, internalIP));
    }

    fetch_window currBatch;
    cache.prepareForOps(internalIP);
    bool fwAlreadyInCache = cache.isAHit();
    if (fwAlreadyInCache)
    {
        // shortenThisCycleBy(1);
        currBatch = cache.get();
    }
    else
    {
        currBatch = getFetchWindowFromLS(internalIP);
        cache.store(currBatch);
    }
    SynchronizedDataPackage<fetch_window> syncResponse(currBatch, internalIP);
    internalIP += FETCH_WINDOW_BYTES;
    
    clock_time lastTick = waitTillLastTick();
    syncResponse.sentAt = lastTick;
    if (!fromMetoDE->pendingB())
    {
        fromMetoDE->sendA(syncResponse);
        if (clockSyncVars->running)
        {
            logComplete(lastTick, log(LoggablePackage(internalIP - FETCH_WINDOW_BYTES, currBatch)));
            if (fwAlreadyInCache)
                logAdditional("\t(From IC's cache)\n");
        }
    }
}

void InstructionCache::run()
{
    std::string startMessage = "\t!IC signals beginning of simulation at T=" + std::to_string(getCurrTime()) + " from #fff0!\n";
    logAdditional(startMessage);
    clockSyncVars->ICReady = true;
    startCurrOpTimer();
    executeModuleLogic();
    IClockBoundModule::run();
}