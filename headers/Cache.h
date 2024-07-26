#pragma once

#include "CacheElem.h"

#include <unordered_map>

template <typename Id, typename Value>
class Cache
{
private:
    std::unordered_map<Id, CacheElement<Value>> storage;
    byte maxSize;
    clock_time staleCacheBias;

public:
    Cache<Id,Value>(byte maxSize, clock_time staleCacheBias):
        maxSize(maxSize), staleCacheBias(staleCacheBias) {}; 
    
    bool hasCached(Id id)
    {
        auto valFound = storage.find(id);
        return valFound != storage.end();
    }

    bool isFull()
    {
        return storage.size() == maxSize;
    }

    // It is assumed hasCached(id) has returned true prior to calling this
    Value fetchAndUpdate(Id id, clock_time newHitTime)
    {
        auto foundElem = storage.find(id);
        foundElem->second.updateLastHitTime(newHitTime);
        return foundElem->second.data;
    }

    byte size()
    {
        return storage.size();
    }

    // Also assumed id not already cached
    void tryCache(Id id, Value value, clock_time firstHitTime)
    {
        if (!isFull())
        {
            storage.insert({id, CacheElement<Value>(value, firstHitTime)});
            return;
        }

        const Id* eliminationCandidate = nullptr;
        for (auto elem = storage.begin(); elem != storage.end(); ++elem)
            if (firstHitTime - elem->second.lastHitTime >= staleCacheBias &&
                (eliminationCandidate == nullptr || elem->second.lastHitTime < storage.at(*eliminationCandidate).lastHitTime))
                eliminationCandidate = &elem->first;
        if (eliminationCandidate == nullptr)
            return;

        storage.erase(*eliminationCandidate);
        storage.insert({id, CacheElement<Value>(value, firstHitTime)});
    }
};