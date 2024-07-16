#pragma once

#include "Config.h"

#include <condition_variable>

class ClockSyncPackage
{
public:
    clock_time cycleCount;
    std::condition_variable update;
    std::mutex updateLock;
    bool running;

    ClockSyncPackage(): running(true), cycleCount(0) {};
};