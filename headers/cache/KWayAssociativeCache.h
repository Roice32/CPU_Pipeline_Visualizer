#pragma once

#include "KWayCacheSet.h"

#include <cassert>

template <typename DataType>
class TwoWayAssociativeCache
{
private:
    std::vector<KWayCacheSet<DataType>> storage;
    byte cacheSize;
    byte offsetSize;
    byte indexSize;
    byte tagSize;

    byte currReqIndex;
    address currReqTag;
    byte foundIndex;

public:
    TwoWayAssociativeCache<DataType>()
    {
        byte setSize = CACHE_SET_SIZE;
        while (setSize > 1)
        {
            assert(setSize % 2 == 0 && "Set size for k-way cache not power of 2");
            setSize /= 2;
        }

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
            storage.push_back(KWayCacheSet<DataType>());
    }

    void prepareForOps(address currReq)
    {
        currReqIndex = (address (currReq << tagSize)) >> (tagSize + offsetSize);
        currReqTag = currReq >> (indexSize + offsetSize);
        assert(currReqIndex >= 0 && currReqIndex < cacheSize);
        foundIndex = CACHE_SET_SIZE;
    }

    bool isAHit()
    {
        for (byte ind = 0; ind < CACHE_SET_SIZE; ++ind)
            if (storage[currReqIndex].storedLines[ind].tag == currReqTag)
            {
                foundIndex = ind;
                return storage[currReqIndex].storedLines[ind].valid;
            }
        return false;
    }

    DataType get(clock_time hitTime)
    {
        CacheLine<DataType>& target = storage[currReqIndex].storedLines[foundIndex];
        target.lastHitTime = hitTime;
        return target.data;
    }

    void store(DataType newData, clock_time hitTime)
    {
        if (foundIndex != CACHE_SET_SIZE)
        {
            CacheLine<DataType>& target = storage[currReqIndex].storedLines[foundIndex].data;
            if (target.data != newData)
                target.modified = true;
            target.data = newData;
            target.lastHitTime = hitTime;
            target.valid = true;
            return;
        }

        byte elimCandidate = 0;
        std::vector<CacheLine<DataType>>& targetSet = storage[currReqIndex].storedLines;
        for (byte ind = 0; ind < CACHE_SET_SIZE; ++ind)
        {
            if (!targetSet[ind].valid)
            {
                elimCandidate = ind;
                break;
            }
            if (targetSet[ind].lastHitTime < targetSet[elimCandidate].lastHitTime)
                elimCandidate = ind;
        }
        targetSet[elimCandidate].data = newData;
        targetSet[elimCandidate].tag = currReqTag;
        targetSet[elimCandidate].lastHitTime = hitTime;
        targetSet[elimCandidate].modified = false;
        targetSet[elimCandidate].valid = true;
    }

    void invalidate()
    {
        if (foundIndex == CACHE_SET_SIZE)
            return;
        storage[currReqIndex].storedLines[foundIndex].valid = false;
    }
};