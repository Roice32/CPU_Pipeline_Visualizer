#pragma once

#include "Config.h"

#include <condition_variable>

class ClockSyncPackage
{
public:
  clock_time cycleCount;
  std::condition_variable update;
  std::mutex updateLock;
  bool ICReady;
  bool running;

  ClockSyncPackage(): running(true), ICReady(false), cycleCount(0) {};
};