#pragma once

#include "CacheLine.h"
#include <vector>

template <typename DataType>
class Cache
{
private:
    std::vector<CacheLine<DataType>> storage;
    byte offsetSize;
    byte indexSize;
    byte tagSize;

public:
    Cache<DataType>()
    {
        byte cacheSize = CACHE_WORDS_SIZE * WORD_BYTES / sizeof(DataType);
        
        offsetSize = 1;
        byte bytesReachable = 1;
        while (bytesReachable < sizeof(DataType))
        {
            bytesReachable *= 2;
            ++offsetSize;
        }

        indexSize = 1;
        byte maxIndex = 2;
        while (maxIndex < cacheSize)
        {
            maxIndex *= 2;
            ++indexSize;
        }

        tagSize = sizeof(address) * 8 - indexSize - offsetSize;

        for (byte ind = 0; ind < cacheSize; ++ind)
            storage.push_back(CacheLine<DataType>());
    }

    bool isAHit(address request)
    {
        byte index = (request << tagSize) >> (tagSize + offsetSize);
        address tag = request >> (indexSize + offsetSize);
        return storage[index].tag == tag && storage[index].validBit;
    }

    DataType get(address request)
    {
        byte index = (request << tagSize) >> (tagSize + offsetSize);
        address tag = request >> (indexSize + offsetSize);
        return storage[index].data;
    }

    void store(DataType data, address addr)
    {
        byte index = (addr << tagSize) >> (tagSize + offsetSize);
        address tag = addr >> (indexSize + offsetSize);
        storage[index] = CacheLine<DataType>(tag, data);
    }

    void invalidate(address addr)
    {
        byte index = (addr << tagSize) >> (tagSize + offsetSize);
        address tag = addr >> (indexSize + offsetSize);
        if (storage[index].tag == tag)
            storage[index].validBit = false;
    }
};