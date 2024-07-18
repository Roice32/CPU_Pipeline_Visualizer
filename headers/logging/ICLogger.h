#pragma once

#include "ILogger.cpp"

class ICLogger: public ILogger
{
private:
    char* convDecToHex(word source, char* dest);

public:
    ICLogger();
    void log(LoggablePackage toLog) override;
};