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
    cacheSize = IC_CACHE_WORDS_SIZE * WORD_BYTES / sizeof(DataType);
    
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
    return storage[currReqIndex].tag == currReqTag && storage[currReqIndex].valid;
  }

  DataType get()
  {
    return storage[currReqIndex].data;
  }

  address store(DataType data)
  {
    address discardedAddr = DUMMY_ADDRESS;
    if (isAHit())
      discardedAddr = (storage[currReqIndex].tag << (indexSize + offsetSize)) | (currReqIndex << offsetSize);

    storage[currReqIndex].data = data;
    storage[currReqIndex].tag = currReqTag;
    storage[currReqIndex].valid = true;

    return discardedAddr;
  }

  bool invalidate()
  {
    if (storage[currReqIndex].tag == currReqTag)
    {
      storage[currReqIndex].valid = false;
      return true;
    }
    return false;
  }

  byte getCurrReqIndex() const
  {
    return currReqIndex;
  }
};