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

  while (*selfRunning)  // The Clock is still meant to run
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(CLOCK_PERIOD_MILLIS));
    { // clockSyncVars holds the mutex and condition variable for synchronization, shared by the threads
      // Place a lock to prevent other threads from reading the cycle count while it is being updated
      std::lock_guard<std::mutex> lockdown(clockSyncVars->updateLock);
      ++(clockSyncVars->cycleCount);

      if (clockSyncVars->cycleCount > CYCLES_LIMIT)
      {
        recorder->setSimEndReason(CYCLE_LIMIT_EXCEEDED);
        clockSyncVars->running = false;  // Stop the simulation for all threads
      }
      else if (clockSyncVars->running)  // No end_sim or Double Exception encountered yet
      {
        recorder->goToNextState();  // Record the transition to the next simulation state
      }
    }
    // Wake up all threads waiting for the clock signal
    clockSyncVars->update.notify_all();
  }
}