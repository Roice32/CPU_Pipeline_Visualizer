#include "LSLogger.h"

LSLogger::LSLogger():
    ILogger("LS") {};

std::string LSLogger::vectorToString(std::vector<word> vec)
{
    std::string result = "{ ";
    for (byte ind = 0; ind < vec.size(); ++ind)
        result += convDecToHex(vec.at(ind)) + " ";
    result += "}";
    return result;
}

std::string LSLogger::logAccept(address reqAddr, bool isFromEX)
{
    std::string result = "Accepted request from ";
    result += (isFromEX ? "EX" : "IC");
    result += " regarding #" + convDecToHex(reqAddr) + "\n";
    return result;
}

std::string LSLogger::log(LoggablePackage toLog)
{
    std::string result = "";
    if (toLog.wasStoreOp)
    {
        result = "Stored " + vectorToString(toLog.data) + " at #" + convDecToHex(toLog.ip) + " for EX\n";
        return result;
    }
    result = "Fetched ";
    if (toLog.wasForEX)
        result += vectorToString(toLog.data);
    else
        result += fetchWindowToString(toLog.fetchWindow);
    result += " from #" + convDecToHex(toLog.ip) + " for " + (toLog.wasForEX ? "EX" : "IC") + "\n";
    return result;
}