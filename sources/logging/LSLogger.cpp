#include "LSLogger.h"

LSLogger::LSLogger():
    ILogger("LS") {};

void LSLogger::printVector(std::vector<word> vec)
{
    char valueInHex[WORD_BYTES * 2 + 1];
    printf("{ ");
    for (byte ind = 0; ind < vec.size(); ++ind)
        printf("%s ", convDecToHex(vec.at(ind), valueInHex));
    printf("}");
}

void LSLogger::logAccept(clock_time timestamp, address reqAddr, bool isFromEX)
{
    char addrInHex[ADDRESS_WIDTH / 4 + 1] = "";
    printf("[LS@T=%lu]> Accepted request from %s regarding address #%s\n", timestamp, isFromEX ? "EX" : "IC", convDecToHex(reqAddr, addrInHex));
}

void LSLogger::log(LoggablePackage toLog)
{
    char hexAddr[ADDRESS_WIDTH / 4];
    if (toLog.wasStoreOp)
    {
        printf("Stored ");
        printVector(toLog.data);
        printf(" at #%s for EX\n", convDecToHex(toLog.ip, hexAddr));
        return;
    }
    printf("Fetched ");
    if (toLog.wasForEX)
        printVector(toLog.data);
    else
        printFetchWindow(toLog.fetchWindow);
    printf(" from #%s for %s\n", convDecToHex(toLog.ip, hexAddr), toLog.wasForEX ? "EX" : "IC");
}