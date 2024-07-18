#pragma once

#include "ILogger.cpp"

class DELogger: public ILogger
{
public:
    DELogger();
    void log(LoggablePackage toLog) override;
    void logJump(clock_time timestamp, address newAddr);
    void logDiscard(clock_time timestamp, address ignored, address awaited);
};