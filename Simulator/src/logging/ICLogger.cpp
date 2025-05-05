#include "ICLogger.h"

ICLogger::ICLogger():
  ILogger("IC") {};

std::string ICLogger::log(LoggablePackage toLog)
{
  std::string result = "Delivered " + fetchWindowToString(toLog.fetchWindow) + " from #" + convDecToHex(toLog.ip) + " to DE\n";
  return result;
}

std::string ICLogger::logRequest(address requested)
{
  std::string result = "Requested fetch window starting at #" + convDecToHex(requested) + " from LS\n";
  return result;
}

std::string ICLogger::logJump(address addr, address alignedAddr)
{
  std::string result = "Received signal to change IP to #" + convDecToHex(addr) + " (aligned as #" + convDecToHex(alignedAddr) + ")\n";
  return result;
}