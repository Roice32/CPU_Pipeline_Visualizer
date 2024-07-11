#include "InstructionCache.h"
#include "Decode.h"

InstructionCache::InstructionCache(InterThreadCommPipe<address, fetch_window>* commPipeWithLS, register_16b* ip): 
    requestsToLS(commPipeWithLS), IP(ip) {};

void InstructionCache::requestFetchWindow() {
    if (*IP == 0xffff)
        return;
    requestsToLS->sendRequest(*IP);
    // TO DO: replace this with promises in the future
    while (!requestsToLS->pendingResponse()) ;
    currBatch = requestsToLS->getResponse();
    // temp
    passForDecode();
}

void InstructionCache::passForDecode() {
    DEModule->processFetchWindow(currBatch);
}

void InstructionCache::setDEModule(Decode* deModuleRef)
{
    DEModule = deModuleRef;
}