#pragma once

#include "ILogger.h"

class LSLogger: public ILogger
{
private:
  std::string vectorToString(std::vector<word> vec);

protected:
  LSLogger();
  std::string logAccept(address reqAddr, bool isFromEX);
  std::string log(LoggablePackage toLog) override;
};