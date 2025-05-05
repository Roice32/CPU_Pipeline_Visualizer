#pragma once

#include "../Config.h"

template <typename DataType>
class CacheLine
{
public:
  DataType data;
  address tag;
  clock_time lastHitTime;
  bool valid;
  bool modified;

  CacheLine<DataType>(): valid(false), modified(false), lastHitTime(0) {};
};