#pragma once

#include "ILogger.cpp"

class ICLogger: public ILogger
{
public:
    ICLogger();
    void log(LoggablePackage toLog) override;
    void logRequest(clock_time timestamp, address requested);
    void logJump(clock_time timestamp, address addr, address alignedAddr);
};