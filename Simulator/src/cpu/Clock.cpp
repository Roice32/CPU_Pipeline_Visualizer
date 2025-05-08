#include "Clock.h"

#include <cstdio>
#include <thread>

Clock::Clock(std::shared_ptr<ExecutionRecorder> recorder):
  clockSyncVars(std::make_shared<ClockSyncPackage>()),
  selfRunning(std::make_shared<bool>(true)),
  recorder(recorder) {};

void Clock::run()
{
  while(!clockSyncVars->ICReady)
    std::this_thread::sleep_for(std::chrono::milliseconds(CLOCK_PERIOD_MILLIS));
  while (*selfRunning)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(CLOCK_PERIOD_MILLIS));
    {
      std::lock_guard<std::mutex> lockdown(clockSyncVars->updateLock);
      ++clockSyncVars->cycleCount;
      recorder->goToNextState();
    }
    clockSyncVars->update.notify_all();
  }
}