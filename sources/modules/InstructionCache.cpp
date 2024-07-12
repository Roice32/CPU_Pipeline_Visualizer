#include "InstructionCache.h"

InstructionCache::InstructionCache(std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithLS, std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithDE, std::shared_ptr<register_16b> flags): 
    requestsToLS(commPipeWithLS), requestsFromDE(commPipeWithDE), flags(flags) {};

fetch_window InstructionCache::getFetchWindowFromLS(address addr) {
    requestsToLS->sendRequest(addr);
    //requestsToLS->sendRequest(addr / 4 * 4); <- Sth like this when IC is snapped to 64b
    // TO DO: replace this with promises in the future
    while (!requestsToLS->pendingResponse()) ;
    return requestsToLS->getResponse();
}

void InstructionCache::run()
{
    address reqAddress;
    fetch_window currBatch;
    while (*flags & RUNNING)
    {
        // TO DO: Futures
        if (!requestsFromDE->pendingRequest())
            continue;
        reqAddress = requestsFromDE->getRequest();
        currBatch = getFetchWindowFromLS(reqAddress);
        requestsFromDE->sendResponse(currBatch);
    }
}