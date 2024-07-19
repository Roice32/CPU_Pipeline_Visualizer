#pragma once

#include "ILogger.cpp"

class LSLogger: public ILogger
{
private:
    void printVector(std::vector<word> vec);

protected:
    LSLogger();
    void logAccept(clock_time timestamp, address reqAddr, bool isFromEX);
    void log(LoggablePackage toLog) override;
};