#include "Clock.h"

#include <cstdio>
#include <thread>

Clock::Clock(): clockSyncVars(std::make_shared<ClockSyncPackage>()) {};

void Clock::run()
{
    // TO DO: Fix the clock stopping leaving other threads starving
    while (clockSyncVars->running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(CLOCK_PERIOD_MILLIS));
        {
            std::lock_guard<std::mutex> lockdown(clockSyncVars->updateLock);
            ++clockSyncVars->cycleCount;
        }
        clockSyncVars->update.notify_all();
    }
}