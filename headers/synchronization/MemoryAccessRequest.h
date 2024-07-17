#pragma once

#include "Config.h"

#include <vector>

class MemoryAccessRequest
{
public:
    address reqAddr;
    byte wordsSizeOfReq;
    bool isStoreOperation;
    std::vector<word> reqData;

    MemoryAccessRequest(address addr, byte howManyWords, bool storeOp = false, std::vector<word> data = {})
    {
        reqAddr = addr;
        wordsSizeOfReq = howManyWords;
        isStoreOperation = storeOp;
        if (isStoreOperation)
            reqData = data;
    }
};