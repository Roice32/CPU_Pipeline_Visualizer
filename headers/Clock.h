#pragma once

#include "ClockSyncPackage.h"

class Clock
{
public:
    std::shared_ptr<ClockSyncPackage> clockSyncVars;

    Clock();
    void run();
};