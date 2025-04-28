#pragma once

#include "../synchronization/ClockSyncPackage.h"

class Clock
{
public:
    std::shared_ptr<ClockSyncPackage> clockSyncVars;
    std::shared_ptr<bool> selfRunning;

    Clock();
    void run();
};