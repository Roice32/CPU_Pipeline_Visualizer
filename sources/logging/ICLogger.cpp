#include "ICLogger.h"

ICLogger::ICLogger():
    ILogger("IC") {};

void ICLogger::log(LoggablePackage toLog)
{
    char addrInHex[ADDRESS_WIDTH / 4];
    printf("Delivered ");
    printFetchWindow(toLog.fetchWindow);
    printf(" from #%s to DE\n", convDecToHex(toLog.ip, addrInHex));
}

void ICLogger::logRequest(clock_time timestamp, address requested)
{
    char addrInHex[ADDRESS_WIDTH / 4 + 1] = "";
    printf("[IC@T=%lu]> Requested fetch window starting at #%s from LS\n", timestamp, convDecToHex(requested, addrInHex));
}

void ICLogger::logJump(clock_time timestamp, address addr, address alignedAddr)
{
    char newAddr[ADDRESS_WIDTH / 4 + 1] = "";
    char alignedNewAddr[ADDRESS_WIDTH / 4 + 1] = "";
    printf("[IC@T=%lu]> Received signal to change IP to #%s (aligned as #%s)\n", timestamp, convDecToHex(addr, newAddr), convDecToHex(alignedAddr, alignedNewAddr));
}