#include "InstructionCache.h"

InstructionCache::InstructionCache(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, address>> commPipeWithDE,
    std::shared_ptr<ClockSyncPackage> clockSyncVars,
    std::shared_ptr<register_16b> ip):
        IClockBoundModule(clockSyncVars, 3, "Instruction Cache"),
        fromMetoLS(commPipeWithLS), fromMetoDE(commPipeWithDE), internalIP(0xfff0) {};

fetch_window InstructionCache::getFetchWindowFromLS(address addr) {
    SynchronizedDataPackage<address> syncReq(addr, clockSyncVars->cycleCount);
    syncReq.sentAt = getCurrTime();
    fromMetoLS->sendA(syncReq);
    logComplete(getCurrTime(), logRequest(internalIP));
    enterIdlingState();
    while (!fromMetoLS->pendingB() && clockSyncVars->running)
        returnFromIdlingState();

    if (!clockSyncVars->running)
        return 0;

    SynchronizedDataPackage<fetch_window> receivedPckg = fromMetoLS->getB();
    awaitNextTickToHandle(receivedPckg);
    return receivedPckg.data;
}

bool InstructionCache::executeModuleLogic()
{
    if (fromMetoDE->pendingB())
    {
        address newAddr = fromMetoDE->getB();
        internalIP = newAddr / FETCH_WINDOW_BYTES * FETCH_WINDOW_BYTES;
        logComplete(getCurrTime(), logJump(newAddr, internalIP));
    }

    fetch_window currBatch;
    currBatch = getFetchWindowFromLS(internalIP);
    SynchronizedDataPackage<fetch_window> syncResponse(currBatch, internalIP);
    internalIP += FETCH_WINDOW_BYTES;
    
    waitTillLastTick();
    clock_time lastTick = getCurrTime();
    syncResponse.sentAt = lastTick;
    fromMetoDE->sendA(syncResponse);
    if (clockSyncVars->running)
        logComplete(lastTick, log(LoggablePackage(internalIP - FETCH_WINDOW_BYTES, currBatch)));
    return true;
}

void InstructionCache::run()
{
    startCurrOpTimer();
    std::string startMessage = "\t!IC begins simulation at T=" + std::to_string(getCurrTime()) + " from #fff0!\n";
    logAdditional(startMessage);
    executeModuleLogic();
    IClockBoundModule::run();
}