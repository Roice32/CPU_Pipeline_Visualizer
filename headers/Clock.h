#pragma once

#include "Config.h"
#include "ClockSyncPackage.h"

class Clock
{
public:
    std::shared_ptr<ClockSyncPackage> clockSyncVars;

    Clock(std::shared_ptr<register_16b> flags);
    void run();
};