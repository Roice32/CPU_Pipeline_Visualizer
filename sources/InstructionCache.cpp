#include "InstructionCache.h"

InstructionCache::InstructionCache(LoadStore* lsModuleRef, register_16b* ip): IFetchWindowRequester(lsModuleRef), IP(ip) {};

void InstructionCache::requestFetchWindow(address addr) {
    currBatch = LSModule->bufferedLoadFrom(*IP);
}

void InstructionCache::passForDecode() {
    DEModule->processFetchWindow(currBatch);
}