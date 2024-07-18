#include "ICLogger.h"

ICLogger::ICLogger():
    ILogger("IC") {};

void ICLogger::log(LoggablePackage toLog)
{
    char addrInHex[ADDRESS_WIDTH / 4];
    printf(" Delivered ");
    printFetchWindow(toLog.fetchWindow);
    printf(" from #%s\n", convDecToHex(toLog.ip, addrInHex));
}