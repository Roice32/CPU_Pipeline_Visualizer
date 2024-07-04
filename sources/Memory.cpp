#include "../headers/Memory.h"
#include "../Config.h"

Memory::Memory(char* hexSourceFilePath)
{
    
    data = new byte[MEMORY_SIZE];
}

byte Memory::getMemoryCell(address addr)
{
    return data[addr];
}

void Memory::setMemoryCell(address addr, byte value)
{
    data[addr] = value;
}

Memory::~Memory()
{
    delete[] data;
}