#include "InstructionCache.h"
#include "Decode.h"
#include <cstdio>

InstructionCache::InstructionCache(LoadStore* lsModuleRef, register_16b* ip): IFetchWindowRequester(lsModuleRef), IP(ip) {};

void InstructionCache::requestFetchWindow() {
    if (*IP == 0xffff)
        return;
    currBatch = LSModule->bufferedLoadFrom(*IP);
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