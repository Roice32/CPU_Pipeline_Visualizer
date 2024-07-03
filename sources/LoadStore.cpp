#include "../headers/LoadStore.h"
#include "../Config.h"

unsigned char LoadStore::loadFrom(unsigned short addr)
{
    return target->getMemoryCell(addr);
}

unsigned int LoadStore::bufferedLoadFrom(unsigned short addr)
{
    unsigned int bufferedFetchResult = 0;
    for (unsigned char currByte = 0; currByte < FETCH_WINDOW_BYTES; ++currByte)
    {
        bufferedFetchResult |= target->getMemoryCell(addr + currByte);
        bufferedFetchResult <<= 8;
    }
    return bufferedFetchResult;
}

void LoadStore::storeAt(unsigned short addr, unsigned char value)
{
    target->setMemoryCell(addr, value);
}