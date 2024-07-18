#pragma once

#include "ILogger.cpp"

class LSLogger: public ILogger
{
private:
    void printVector(std::vector<word> vec);

protected:
    LSLogger();
    void log(LoggablePackage toLog) override;
};