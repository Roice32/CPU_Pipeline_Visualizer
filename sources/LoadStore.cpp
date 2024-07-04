#include "../headers/LoadStore.h"
#include "../Config.h"

byte LoadStore::loadFrom(unsigned short addr)
{
    return target->getMemoryCell(addr);
}

fetch_window LoadStore::bufferedLoadFrom(address addr)
{
    fetch_window bufferedFetchResult = 0;
    for (byte currByte = 0; currByte < FETCH_WINDOW_BYTES; ++currByte)
    {
        bufferedFetchResult |= target->getMemoryCell(addr + currByte);
        bufferedFetchResult <<= 8;
    }
    return bufferedFetchResult;
}

void LoadStore::storeAt(address addr, byte value)
{
    target->setMemoryCell(addr, value);
}