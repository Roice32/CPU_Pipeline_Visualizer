#include "../headers/LoadStore.h"

unsigned char LoadStore::loadFrom(unsigned short addr)
{
    return target->getMemoryCell(addr);
}

void LoadStore::storeAt(unsigned short addr, unsigned char value)
{
    target->setMemoryCell(addr, value);
}