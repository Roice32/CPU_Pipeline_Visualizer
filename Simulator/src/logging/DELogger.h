#pragma once

#include "../interfaces/ILogger.cpp"

class DELogger: public ILogger
{
public:
    DELogger();
    std::string log(LoggablePackage toLog) override;
    std::string logJump(address newAddr);
    std::string logDiscard(address ignored, address awaited);
};