#pragma once

#include "LoggablePackage.h"

#include <cstdio>

class ILogger
{
private:
    const char* moduleName;

protected:
    ILogger(const char* moduleName): moduleName(moduleName) {};

    void logComplete(clock_time timestamp, LoggablePackage toLog)
    {
        printf("[%s@T=%lu]>", moduleName, timestamp);
        log(toLog);
    }

    virtual void log(LoggablePackage toLog) = 0;
};