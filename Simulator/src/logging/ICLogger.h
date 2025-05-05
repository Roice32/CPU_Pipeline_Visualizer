#pragma once

#include "../interfaces/ILogger.cpp"

class ICLogger: public ILogger
{
public:
    ICLogger();
    std::string log(LoggablePackage toLog) override;
    std::string logRequest(address requested);
    std::string logJump(address addr, address alignedAddr);
};