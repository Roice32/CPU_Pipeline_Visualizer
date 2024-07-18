#include "ICLogger.h"

ICLogger::ICLogger():
    ILogger("IC") {};

void ICLogger::log(LoggablePackage toLog)
{
    char valuesInHex[FETCH_WINDOW_BYTES / WORD_BYTES];
    char addrInHex[ADDRESS_WIDTH / 4];
    printf(" Delivered [");
    byte wordsPerFW = FETCH_WINDOW_BYTES / WORD_BYTES;
    for (byte ind = 0; ind < wordsPerFW; ++ind)
    {
        printf("%s", convDecToHex(toLog.fetchWindow >> ((wordsPerFW - ind - 1) * 16), valuesInHex));
        if (ind != wordsPerFW - 1)
            printf(" ");
    }
    printf("] from #%s\n", convDecToHex(toLog.ip, addrInHex));
}