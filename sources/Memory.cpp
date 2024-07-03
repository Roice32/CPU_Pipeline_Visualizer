#include "../headers/Memory.h"
#include "../Config.h"

Memory::Memory()
{
    data = new unsigned char[MEMORY_SIZE];
}

unsigned char Memory::getMemoryCell(unsigned short addr)
{
    return data[addr];
}

void Memory::setMemoryCell(unsigned short addr, unsigned char value)
{
    data[addr] = value;
}

Memory::~Memory()
{
    delete[] data;
}