#pragma once

#include "Config.h"
#include <cassert>
#include <vector>

template <template <typename DataType> typename Container, typename DataType>
class ICache
{
protected:
    std::vector<Container<DataType>> storage;
    byte cacheSize;
    byte offsetSize;
    byte indexSize;
    byte tagSize;

    byte currReqIndex;
    byte currReqTag;
    bool currHit;

public:
    ICache<Container, DataType>()
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
            storage.push_back(Container<DataType>());
    }

    void prepareForOps(address currReq)
    {
        currReqIndex = (address (currReq << tagSize)) >> (tagSize + offsetSize);
        currReqTag = currReq >> (indexSize + offsetSize);
        assert(currReqIndex >= 0 && currReqIndex < cacheSize);
    }

    virtual bool isAHit() = 0;

    virtual DataType get(clock_time newHitTime = 0) = 0;

    virtual void store(DataType data, clock_time newHitTime = 0) = 0;

    virtual void invalidate() = 0;
};