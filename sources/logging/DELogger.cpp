#include "DELogger.h"

DELogger::DELogger():
    ILogger("DE") {};

std::string DELogger::log(LoggablePackage toLog)
{
    std::string result = "Decoded '" + plainInstructionToString(toLog.instr) + "' from #" + convDecToHex(toLog.ip) + "\n";
    return result;
}

std::string DELogger::logJump(address newAddr)
{
    std::string result = "Received signal to jump to #" + convDecToHex(newAddr) + "; Sent signal furter to IC\n";
    return result;
}

std::string DELogger::logDiscard(address ignored, address awaited)
{
    std::string result = "Ignored fetch window with start address #" + convDecToHex(ignored) + " (awaiting one containing #" + convDecToHex(awaited) + ")\n";
    return result;
}