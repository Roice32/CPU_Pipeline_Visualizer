#pragma once

#include "ClockSyncPackage.h"

#include <memory>

// TO DO: access modifiers clean-up
class IClockBoundModule
{
protected:
    const char* moduleName; // Used mainly for debug.
    std::shared_ptr<ClockSyncPackage> clockSyncVars;
    byte clockTicksPerCycle;
    clock_time startTimeOfCurrOp;
    byte elapsedTimeOfCurrOp;

public:
    IClockBoundModule(std::shared_ptr<ClockSyncPackage> clockSyncVars, byte clockTicksPerCycle, const char* moduleName):
        clockSyncVars(clockSyncVars), clockTicksPerCycle(clockTicksPerCycle), moduleName(moduleName) {};

    void awaitClockSignal()
    {
        std::unique_lock awaitingLock(clockSyncVars->updateLock);
        clockSyncVars->update.wait(awaitingLock);
    }

    void startCurrOpTimer()
    {
        awaitClockSignal();
        startTimeOfCurrOp = clockSyncVars->cycleCount;
        elapsedTimeOfCurrOp = 0;
    }

    void enterIdlingState()
    {
        awaitClockSignal();
        elapsedTimeOfCurrOp += clockSyncVars->cycleCount - startTimeOfCurrOp;
        printf("\t(%s enters idling at T=%lu, having used %hu ticks so far.)\n", moduleName, clockSyncVars->cycleCount, elapsedTimeOfCurrOp);
    }

    void returnFromIdlingState()
    {
        awaitClockSignal();
        startTimeOfCurrOp = clockSyncVars->cycleCount;
        printf("\t(%s resumes execution at T=%lu, having %hu ticks left.)\n", moduleName, clockSyncVars->cycleCount, clockTicksPerCycle - elapsedTimeOfCurrOp);
    }

    void waitTillLastTick()
    {
        if (elapsedTimeOfCurrOp == clockTicksPerCycle - 1)
            return;
        awaitClockSignal();
        elapsedTimeOfCurrOp += clockSyncVars->cycleCount - startTimeOfCurrOp;
        while ((elapsedTimeOfCurrOp < clockTicksPerCycle - 1) && (clockSyncVars->running))
        {
            awaitClockSignal();
            ++elapsedTimeOfCurrOp;
        }
    }

    virtual bool executeModuleLogic() = 0;

    void run()
    {
        bool moduleDidSomething;
        while(clockSyncVars->running)
        {
            startCurrOpTimer();
            printf("(%s STARTS op at T=%lu)\n", moduleName, clockSyncVars->cycleCount);
            moduleDidSomething = executeModuleLogic();
            if (!moduleDidSomething)
            {
                printf("\t(%s has nothing to do.)\n", moduleName);
                continue;
            }
            awaitClockSignal();
            printf("(%s FINISHES op at T=%lu).\n", moduleName, clockSyncVars->cycleCount);
        }
    }
};