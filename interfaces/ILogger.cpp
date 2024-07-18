#pragma once

#include "LoggablePackage.h"

class ILogger
{
private:
    virtual void log(LoggablePackage toLog) = 0;
};