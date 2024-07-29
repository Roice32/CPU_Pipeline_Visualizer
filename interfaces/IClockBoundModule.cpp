#pragma once

#include "ClockSyncPackage.h"
#include "SynchronizedDataPackage.h"

// TO DO: access modifiers clean-up
class IClockBoundModule
{
protected:
    std::shared_ptr<ClockSyncPackage> clockSyncVars;
    byte clockTicksPerOperation;
    clock_time startTimeOfCurrOp;
    byte elapsedTimeOfCurrOp;

public:
    IClockBoundModule(std::shared_ptr<ClockSyncPackage> clockSyncVars, byte clockTicksPerOperation):
        clockSyncVars(clockSyncVars), clockTicksPerOperation(clockTicksPerOperation) {};

    clock_time inline getCurrTime()
    {
        return clockSyncVars->cycleCount;
    }

    void inline awaitClockSignal()
    {
        if (!clockSyncVars->running)
            return;
        std::unique_lock awaitingLock(clockSyncVars->updateLock);
        clockSyncVars->update.wait(awaitingLock);
    }

    template <typename DataType>
    void inline awaitNextTickToHandle(SynchronizedDataPackage<DataType> receivedPackage)
    {
        if (receivedPackage.sentAt == clockSyncVars->cycleCount)
        {
            awaitClockSignal();
            ++startTimeOfCurrOp;
        }
    }

    void inline shortenThisCycleBy(byte howManyTicks)
    {
        elapsedTimeOfCurrOp += howManyTicks;
    }

    void inline startCurrOpTimer()
    {
        awaitClockSignal();
        startTimeOfCurrOp = clockSyncVars->cycleCount;
        elapsedTimeOfCurrOp = 0;
    }

    void inline enterIdlingState()
    {
        awaitClockSignal();
        elapsedTimeOfCurrOp += clockSyncVars->cycleCount - startTimeOfCurrOp;
    }

    void inline returnFromIdlingState()
    {
        awaitClockSignal();
        startTimeOfCurrOp = clockSyncVars->cycleCount;
    }

    clock_time waitTillLastTick()
    {
        if (elapsedTimeOfCurrOp >= clockTicksPerOperation - 1)
            return clockSyncVars->cycleCount;
        awaitClockSignal();
        elapsedTimeOfCurrOp += clockSyncVars->cycleCount - startTimeOfCurrOp;
        while ((elapsedTimeOfCurrOp < clockTicksPerOperation - 1) && (clockSyncVars->running))
        {
            awaitClockSignal();
            ++elapsedTimeOfCurrOp;
        }
        return clockSyncVars->cycleCount;
    }

    void inline endSimulation()
    {
        clockSyncVars->running = false;
    }

    virtual void executeModuleLogic() = 0;

    virtual void run()
    {
        while(clockSyncVars->running)
        {
            startCurrOpTimer();
            executeModuleLogic();
        }
    }
};