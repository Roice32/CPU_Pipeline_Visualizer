#include "DELogger.h"

DELogger::DELogger():
    ILogger("DE") {};

void DELogger::log(LoggablePackage toLog)
{
    char hexAddr[ADDRESS_WIDTH / 4 + 1];
    printf("Decoded '");
    printPlainInstruction(toLog.instr);
    printf("' from #%s\n", convDecToHex(toLog.ip, hexAddr));
}

void DELogger::logJump(clock_time timestamp, address newAddr)
{
    char addrHex[ADDRESS_WIDTH / 4 + 1] = "";
    printf("[DE@T=%lu]> Received signal of jump to #%s; Sent signal further to IC\n", timestamp, convDecToHex(newAddr, addrHex));
}

void DELogger::logDiscard(clock_time timestamp, address ignored, address awaited)
{
    char ignoredAddr[ADDRESS_WIDTH / 4 + 1] = "";
    char awaitedAddr[ADDRESS_WIDTH / 4 + 1] = "";
    printf("[DE@T=%lu]> Ignored fetch window with start address #%s (awaiting one containing #%s)\n",
        timestamp, convDecToHex(ignored, ignoredAddr), convDecToHex(awaited, awaitedAddr));
}