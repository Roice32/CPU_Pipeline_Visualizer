#pragma once

#include "CacheLine.h"

#include <vector>

template <typename DataType>
class KWayCacheSet
{
public:
  std::vector<CacheLine<DataType>> storedLines;

  KWayCacheSet<DataType>()
  {
    for(byte ind = 0; ind < LS_CACHE_SET_ENTRIES_COUNT; ++ind)
      storedLines.push_back(CacheLine<DataType>());
  }

  friend class ExecutionRecorder;
};