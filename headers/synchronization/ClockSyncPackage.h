#pragma once

#include "Config.h"

#include <memory>
#include <condition_variable>

class ClockSyncPackage
{
public:
    std::shared_ptr<register_16b> flags;
    clock_time cycleCount;
    std::condition_variable update;
    std::mutex updateLock;

    ClockSyncPackage(std::shared_ptr<register_16b> flags): flags(flags), cycleCount(0) {};
};