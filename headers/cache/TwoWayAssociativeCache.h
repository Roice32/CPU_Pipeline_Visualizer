#pragma once

#include "TwoWayCacheSet.h"

#include <cassert>
#include <vector>

template <typename DataType>
class TwoWayAssociativeCache
{
private:
    std::vector<TwoWayCacheSet<DataType>> storage;
    byte cacheSize;
    byte offsetSize;
    byte indexSize;
    byte tagSize;

    byte currReqIndex;
    address currReqTag;

public:
    TwoWayAssociativeCache<DataType>()
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

        --indexSize;

        tagSize = sizeof(address) * 8 - indexSize - offsetSize;

        for (byte ind = 0; ind < cacheSize; ++ind)
            storage.push_back(TwoWayCacheSet<DataType>());
    }

    void prepareForOps(address currReq)
    {
        currReqIndex = (address (currReq << tagSize)) >> (tagSize + offsetSize);
        currReqTag = currReq >> (indexSize + offsetSize);
        assert(currReqIndex >= 0 && currReqIndex < cacheSize);
    }

    bool isAHit()
    {
        bool firstHit = storage[currReqIndex].first.tag == currReqTag && storage[currReqIndex].validBit;
        bool secondHit = storage[currReqIndex].second.tag == currReqTag && storage[currReqIndex].validBit;
        return firstHit || secondHit;
    }

    DataType get(clock_time hitTime)
    {
        bool firstHit = storage[currReqIndex].first.tag == currReqTag && storage[currReqIndex].validBit;
        if (firstHit)
        {
            storage[currReqIndex].first.lastHitTime = hitTime;
            return storage[currReqIndex].first.data;
        }
        storage[currReqIndex].second.lastHitTime = hitTime;
        return storage[currReqIndex].second.data;
    }

    void store(DataType data, clock_time hitTime)
    {
        bool firstSlotFree = !storage[currReqIndex].first.validBit;
        bool secondSlotFree = !storage[currReqIndex].second.validBit;
        bool firstEntryIsOlder = !firstSlotFree &&
            !secondSlotFree &&
            storage[currReqIndex].first.lastHitTime < storage[currReqIndex].second.lastHitTime;

        if (firstSlotFree || firstEntryIsOlder)
        {
            storage[currReqIndex].first.data = data;
            storage[currReqIndex].first.tag = currReqTag;
            storage[currReqIndex].first.lastHitTime = hitTime;
            storage[currReqIndex].first.validBit = true;
            return;
        }
        storage[currReqIndex].second.data = data;
        storage[currReqIndex].second.tag = currReqTag;
        storage[currReqIndex].second.lastHitTime = hitTime;
        storage[currReqIndex].second.validBit = true;
    }

    void invalidate()
    {
        bool firstHit = storage[currReqIndex].first.tag == currReqTag && storage[currReqIndex].second.validBit;
        if (firstHit)
        {
            storage[currReqIndex].first.validBit = false;
            return;
        }
        bool secondHit = storage[currReqIndex].second.tag == currReqTag && storage[currReqIndex].second.validBit;
        if (secondHit)
            storage[currReqIndex].second.validBit = false;
    }
};