#include "InstructionCache.h"

InstructionCache::InstructionCache(std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithDE,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
        IClockBoundModule(clockSyncVars, 3, "Instruction Cache"),
        requestsToLS(commPipeWithLS), requestsFromDE(commPipeWithDE) {};

fetch_window InstructionCache::getFetchWindowFromLS(address addr) {
    requestsToLS->sendRequest(addr);
    enterIdlingState();
    // TO DO: replace this with promises in the future
    while (!requestsToLS->pendingResponse() && clockSyncVars->running) ;
    returnFromIdlingState();
    return requestsToLS->getResponse();
}

bool InstructionCache::executeModuleLogic()
{
    if (!requestsFromDE->pendingRequest())
        return false;
    address reqAddress;
    fetch_window currBatch;
    reqAddress = requestsFromDE->getRequest();
    currBatch = getFetchWindowFromLS(reqAddress);
    waitTillLastTick();
    requestsFromDE->sendResponse(currBatch);
    return true;
}