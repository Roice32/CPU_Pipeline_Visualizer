#include "DELogger.h"

DELogger::DELogger():
    ILogger("DE") {};

void DELogger::log(LoggablePackage toLog)
{
    char hexAddr[ADDRESS_WIDTH / 4 + 1];
    printf(" Decoded '");
    printPlainInstruction(toLog.instr);
    printf("' from #%s\n", convDecToHex(toLog.ip, hexAddr));
}