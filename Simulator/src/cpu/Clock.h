#pragma once

#include "../synchronization/ClockSyncPackage.h"
#include "../logging/ExecutionRecorder.h"

class Clock
{
public:
  std::shared_ptr<ClockSyncPackage> clockSyncVars;
  std::shared_ptr<bool> selfRunning;
  std::shared_ptr<ExecutionRecorder> recorder;

  Clock(std::shared_ptr<ExecutionRecorder> recorder);
  void run();
};