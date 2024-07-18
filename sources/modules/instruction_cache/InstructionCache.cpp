#include "InstructionCache.h"

InstructionCache::InstructionCache(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, bool>> commPipeWithDE,
    std::shared_ptr<ClockSyncPackage> clockSyncVars,
    std::shared_ptr<register_16b> ip):
        IClockBoundModule(clockSyncVars, 3, "Instruction Cache"),
        fromMetoLS(commPipeWithLS), fromMetoDE(commPipeWithDE), IP(ip), internalIP(0xfff0) {};

fetch_window InstructionCache::getFetchWindowFromLS(address addr) {
    SynchronizedDataPackage<address> syncReq(addr, clockSyncVars->cycleCount);
    fromMetoLS->sendA(syncReq);
    enterIdlingState();
    while (!fromMetoLS->pendingB() && clockSyncVars->running)
        awaitClockSignal();
    returnFromIdlingState();
    return fromMetoLS->getB().data;
}

bool InstructionCache::executeModuleLogic()
{
    if (fromMetoDE->pendingB())
    {
        internalIP = *IP / FETCH_WINDOW_BYTES * FETCH_WINDOW_BYTES;
        fromMetoDE->getB();
    }

    fetch_window currBatch;
    currBatch = getFetchWindowFromLS(internalIP);
    SynchronizedDataPackage<fetch_window> syncResponse(currBatch, internalIP);
    internalIP += FETCH_WINDOW_BYTES;
    
    waitTillLastTick();
    syncResponse.sentAt = clockSyncVars->cycleCount;
    fromMetoDE->sendA(syncResponse);
    return true;
}