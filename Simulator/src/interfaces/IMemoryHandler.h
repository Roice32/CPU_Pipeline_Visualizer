#pragma once

#include "Memory.h"

#include <memory>

class IMemoryHandler
{
protected:
  std::shared_ptr<Memory> target;
  
  IMemoryHandler(std::shared_ptr<Memory> target): target(target) {};
  virtual byte loadFrom(address addr) = 0;
  virtual fetch_window bufferedLoadFrom(address addr) = 0;
  virtual void storeAt(address addr, byte value) = 0;
};