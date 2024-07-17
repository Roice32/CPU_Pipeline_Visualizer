#pragma once

#include "ClockSyncPackage.h"
#include "SynchronizedDataPackage.h"

#include <memory>

// TO DO: access modifiers clean-up
class IClockBoundModule
{
protected:
    const char* moduleName; // Used mainly for debug.
    std::shared_ptr<ClockSyncPackage> clockSyncVars;
    byte clockTicksPerOperation;
    clock_time startTimeOfCurrOp;
    byte elapsedTimeOfCurrOp;

public:
    IClockBoundModule(std::shared_ptr<ClockSyncPackage> clockSyncVars, byte clockTicksPerOperation, const char* moduleName):
        clockSyncVars(clockSyncVars), clockTicksPerOperation(clockTicksPerOperation), moduleName(moduleName) {};

    void awaitClockSignal()
    {
        if (!clockSyncVars->running)
            return;
        std::unique_lock awaitingLock(clockSyncVars->updateLock);
        clockSyncVars->update.wait(awaitingLock);
    }

    template <typename DataType>
    void awaitNextTickToHandle(SynchronizedDataPackage<DataType> receivedPackage)
    {
        if (receivedPackage.sentAt == clockSyncVars->cycleCount)
        {
            awaitClockSignal();
            ++startTimeOfCurrOp;
        }
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
        //printf("\t(%s enters idling at T=%lu, having used %hu ticks so far.)\n", moduleName, clockSyncVars->cycleCount, elapsedTimeOfCurrOp);
    }

    void returnFromIdlingState()
    {
        awaitClockSignal();
        startTimeOfCurrOp = clockSyncVars->cycleCount;
        //printf("\t(%s resumes execution at T=%lu, having %hu ticks left.)\n", moduleName, clockSyncVars->cycleCount, clockTicksPerOperation - elapsedTimeOfCurrOp);
    }

    void waitTillLastTick()
    {
        if (elapsedTimeOfCurrOp == clockTicksPerOperation - 1)
            return;
        awaitClockSignal();
        elapsedTimeOfCurrOp += clockSyncVars->cycleCount - startTimeOfCurrOp;
        while ((elapsedTimeOfCurrOp < clockTicksPerOperation - 1) && (clockSyncVars->running))
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
            //printf("(%s STARTS op at T=%lu)\n", moduleName, clockSyncVars->cycleCount);
            moduleDidSomething = executeModuleLogic();
            if (!moduleDidSomething)
            {
                //printf("\t(%s has nothing to do.)\n", moduleName);
                continue;
            }
            awaitClockSignal();
            //printf("(%s FINISHES op at T=%lu).\n", moduleName, clockSyncVars->cycleCount);
        }
        //printf("(%s finished its job.)\n", moduleName);
    }
};