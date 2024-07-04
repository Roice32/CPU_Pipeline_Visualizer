#include "InstructionCache.h"

InstructionCache::InstructionCache(LoadStore* lsModuleRef, register_16b* ip): IFetchWindowRequester(lsModuleRef), IP(ip) {};

fetch_window InstructionCache::requestFetchWindow(address addr) {
    return LSModule->bufferedLoadFrom(*IP);
}