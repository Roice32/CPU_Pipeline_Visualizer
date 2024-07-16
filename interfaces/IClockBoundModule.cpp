#pragma once

#include "ClockSyncPackage.h"

#include <memory>

class IClockBoundModule
{
protected:
    std::shared_ptr<ClockSyncPackage> clockSyncVars;
    byte clockTicksPerCycle;
    clock_time startTimeOfCurrOp;
    clock_time startTimeOfAwaitingOtherModule;
    clock_time endTimeOfAwaitingOtherModule;

public:
    IClockBoundModule(std::shared_ptr<ClockSyncPackage> clockSyncVars, byte clockTicksPerCycle):
        clockSyncVars(clockSyncVars), clockTicksPerCycle(clockTicksPerCycle) {};

    void awaitClockSignal()
    {
        std::unique_lock awaitingLock(clockSyncVars->updateLock);
        clockSyncVars->update.wait(awaitingLock);
    }

    virtual void run() = 0;
};