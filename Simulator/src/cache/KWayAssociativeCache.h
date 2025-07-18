#pragma once

#include "DiscardedCacheElement.h"
#include "KWayCacheSet.h"

#include <unordered_map>

template <typename DataType>
class KWayAssociativeCache
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
  byte elimCandidate;

public:
  KWayAssociativeCache<DataType>()
  {
    byte setSize = LS_CACHE_SET_ENTRIES_COUNT;
    while (setSize > 1)
    {
      if (setSize % 2 != 0)
      {
        std::cerr << "Error: Set size for k-way cache must be a power of 2" << std::endl;
        exit(EXIT_FAILURE);
      }
      setSize /= 2;
    }

    cacheSize = LS_CACHE_WORDS_SIZE * WORD_BYTES / sizeof(DataType) / LS_CACHE_SET_ENTRIES_COUNT;

    offsetSize = 0;
    byte bytesReachable = 1;
    while (bytesReachable < sizeof(DataType))
    {
      bytesReachable *= 2;
      ++offsetSize;
    }

    indexSize = 0;
    byte indexReachable = 1;
    while (indexReachable < cacheSize / LS_CACHE_SET_ENTRIES_COUNT)
    {
      indexReachable *= 2;
      ++indexSize;
    }

    tagSize = sizeof(address) * 8 - indexSize - offsetSize;

    for (byte ind = 0; ind < cacheSize; ++ind)
      storage.push_back(KWayCacheSet<DataType>());
  }

  void prepareForOps(address currReq)
  {
    currReqIndex = (address (currReq << tagSize)) >> (tagSize + offsetSize);
    currReqTag = currReq >> (indexSize + offsetSize);
    if (currReqIndex >= cacheSize)
    {
      std::cerr << "Error: Cache index out of bounds" << std::endl;
      exit(EXIT_FAILURE);
    }
    foundIndex = LS_CACHE_SET_ENTRIES_COUNT;
  }

  bool isAHit()
  {
    for (byte ind = 0; ind < LS_CACHE_SET_ENTRIES_COUNT; ++ind)
      if (storage[currReqIndex].storedLines[ind].tag == currReqTag)
      {
        foundIndex = ind;
        return storage[currReqIndex].storedLines[ind].valid;
      }
    return false;
  }

  DataType get(clock_time hitTime)
  {
    if (foundIndex == LS_CACHE_SET_ENTRIES_COUNT)
    {
      std::cerr << "Error: Attempt to get from cache when no hit recorded" << std::endl;
      exit(EXIT_FAILURE);
    }
    CacheLine<DataType>& target = storage[currReqIndex].storedLines[foundIndex];
    target.lastHitTime = hitTime;
    return target.data;
  }

  DiscardedCacheElement<DataType> store(DataType newData, clock_time hitTime, bool isPlainRead = false)
  {
    if (isAHit() || foundIndex != LS_CACHE_SET_ENTRIES_COUNT)
    {
      CacheLine<DataType>& target = storage[currReqIndex].storedLines[foundIndex];
      if (target.data != newData)
      {
        target.modified = true;
        target.data = newData;
      }
      target.lastHitTime = hitTime;
      target.valid = true;
      return DiscardedCacheElement<DataType>();
    }

    elimCandidate = 0;
    std::vector<CacheLine<DataType>>& targetSet = storage[currReqIndex].storedLines;
    for (byte ind = 0; ind < LS_CACHE_SET_ENTRIES_COUNT; ++ind)
    {
      if (!targetSet[ind].valid)
      {
        elimCandidate = ind;
        break;
      }
      if (targetSet[ind].lastHitTime < targetSet[elimCandidate].lastHitTime)
        elimCandidate = ind;
    }

    DiscardedCacheElement<DataType> eliminatedElement;
    if (targetSet[elimCandidate].valid && targetSet[elimCandidate].modified)
      eliminatedElement = DiscardedCacheElement<DataType>(targetSet[elimCandidate].data,
        ((targetSet[elimCandidate].tag << indexSize) | currReqIndex) << offsetSize);
    else
      eliminatedElement = DiscardedCacheElement<DataType>();

    targetSet[elimCandidate].data = newData;
    targetSet[elimCandidate].tag = currReqTag;
    targetSet[elimCandidate].lastHitTime = hitTime;
    targetSet[elimCandidate].modified = !isPlainRead;
    targetSet[elimCandidate].valid = true;

    return eliminatedElement;
  }

  std::unordered_map<address, DataType> getDataToBeStoredInMemory()
  {
    std::unordered_map<address, DataType> modifiedMem;
    CacheLine<DataType> currElem;
    address currAddr;
    for (byte setInd = 0; setInd < cacheSize; ++setInd)
      for (byte entryInd = 0; entryInd < LS_CACHE_SET_ENTRIES_COUNT; ++entryInd)
      {
        currElem = storage[setInd].storedLines[entryInd];
        if (currElem.valid && currElem.modified)
        {
          currAddr = (currElem.tag << (indexSize + offsetSize)) | (setInd << offsetSize);
          modifiedMem.insert({currAddr, currElem.data});
        }
      }
    return modifiedMem;
  }

  byte getCurrReqTag() {
    return currReqTag;
  }

  byte getCurrReqIndex() {
    return currReqIndex;
  }

  byte getCurrReqInnerIndex() {
    return foundIndex != LS_CACHE_SET_ENTRIES_COUNT ? foundIndex : elimCandidate;
  }
};