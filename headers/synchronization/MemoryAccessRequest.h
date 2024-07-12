#pragma once

#include "Config.h"

class MemoryAccessRequest
{
public:
    bool isStoreOperation;
    address reqAddr;
    word reqData;

    MemoryAccessRequest(address addr, bool storeOp = false, word data = 0)
    {
        isStoreOperation = storeOp;
        reqAddr = addr;
        if (isStoreOperation)
            reqData = data;
    }
};