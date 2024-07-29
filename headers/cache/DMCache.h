#pragma once

#include "CacheLine.h"
#include <cassert>
#include <vector>

template <typename DataType>
class DMCache
{
private:
    std::vector<CacheLine<DataType>> storage;
    byte cacheSize;
    byte offsetSize;
    byte indexSize;
    byte tagSize;

    byte currReqIndex;
    byte currReqTag;

public:
    DMCache<DataType>()
    {
        cacheSize = CACHE_WORDS_SIZE * WORD_BYTES / sizeof(DataType);
        
        offsetSize = 0;
        byte bytesReachable = 1;
        while (bytesReachable < sizeof(DataType))
        {
            bytesReachable *= 2;
            ++offsetSize;
        }

        indexSize = 0;
        byte indexReachable = 1;
        while (indexReachable < cacheSize)
        {
            indexReachable *= 2;
            ++indexSize;
        }

        tagSize = sizeof(address) * 8 - indexSize - offsetSize;

        for (byte ind = 0; ind < cacheSize; ++ind)
            storage.push_back(CacheLine<DataType>());
    }

    void prepareForOps(address currReq)
    {
        currReqIndex = (address (currReq << tagSize)) >> (tagSize + offsetSize);
        currReqTag = currReq >> (indexSize + offsetSize);
        assert(currReqIndex >= 0 && currReqIndex < cacheSize);
    }

    bool isAHit()
    {
        return storage[currReqIndex].tag == currReqTag && storage[currReqIndex].validBit;
    }

    DataType get()
    {
        return storage[currReqIndex].data;
    }

    void store(DataType data)
    {
        storage[currReqIndex].data = data;
        storage[currReqIndex].tag = currReqTag;
        storage[currReqIndex].validBit = true;
    }

    void invalidate()
    {
        if (storage[currReqIndex].tag == currReqTag)
            storage[currReqIndex].validBit = false;
    }
};