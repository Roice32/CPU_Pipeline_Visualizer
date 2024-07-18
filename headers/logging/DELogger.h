#pragma once

#include "ILogger.cpp"

class DELogger: public ILogger
{
public:
    DELogger();
    void log(LoggablePackage toLog) override;
    void logDiscard();
};