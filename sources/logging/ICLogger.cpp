#include "ICLogger.h"

ICLogger::ICLogger():
    ILogger("IC") {};

char* ICLogger::convDecToHex(word source, char* dest)
{
    byte bytesGroup;
    for (byte ind = 0; ind < WORD_BYTES * 2; ++ind)
    {
        bytesGroup = source & 0xf;
        dest[3 - ind] = (bytesGroup > 9) ? ('a' + bytesGroup - 10) : ('0' + bytesGroup);
        source >>= 4;
    }
    return dest;
}

void ICLogger::log(LoggablePackage toLog)
{
    char valuesInHex[FETCH_WINDOW_BYTES / WORD_BYTES];
    printf(" Delivered [");
    byte wordsPerFW = FETCH_WINDOW_BYTES / WORD_BYTES;
    for (byte ind = 0; ind < wordsPerFW; ++ind)
    {
        printf("%s", convDecToHex(toLog.fetchWindow >> ((wordsPerFW - ind - 1) * 16), valuesInHex));
        if (ind != wordsPerFW - 1)
            printf(" ");
    }
    printf("] from #%s\n", convDecToHex(toLog.ip, valuesInHex));
}