#include <fstream>
#include <iostream>

#include "Memory.h"
#include "Config.h"

Memory::Memory(const char* hexSourceFilePath)
{
    data = new byte[MEMORY_SIZE];
    address currAddr = 0x0000;
    char instrLine[6];
    
    std::ifstream sourceCodeFile(hexSourceFilePath);

    if(!sourceCodeFile.is_open())
        std::cout << "X";

    sourceCodeFile.close();
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