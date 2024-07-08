#include <fstream>

#include "Memory.h"
#include "Config.h"

byte Memory::hexCharToDec(const char digit)
{
    return digit - (digit >= 'a' ? 'a' - 10 : '0');
}

void Memory::jumpToNewAddr(address& currAddr, const byte* newAddr)
{
    currAddr = 0x0000;
    for (int i = 0; i < 4; ++i)
    {
        currAddr <<= 4;
        currAddr |= hexCharToDec(newAddr[i]);
    }
}

void Memory::storeData(address& currAddr, const byte* instr)
{
    for (int byteNo = 0; byteNo < 2; ++byteNo)
    {
        byte newData = (hexCharToDec(instr[byteNo * 2]) << 4) | hexCharToDec(instr[byteNo * 2 + 1]);
        this -> setMemoryCell(currAddr++, newData);
    }
}

Memory::Memory(const char* hexSourceFilePath)
{
    data = new byte[MEMORY_SIZE];
    address currAddr = 0x0000;
    byte instrLine[6];
    
    std::ifstream sourceCodeFile(hexSourceFilePath);

    if(!sourceCodeFile.is_open())
        throw 404;

    while(sourceCodeFile >> instrLine)
    {
        if (char (instrLine[0]) == '#')
            jumpToNewAddr(currAddr, instrLine + 1);
        else
            storeData(currAddr, instrLine);
    }

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

void Memory::dumpMemoryContents(char* outputFile)
{
    std::ofstream output(outputFile);
    
    address currCell = 0;
    while (true)
    {
        output << (data[currCell] < 16 ? data[currCell] + '0' : data[currCell] - 10 + 'a')  << ' ';
        ++currCell;
        if (currCell % 256 == 0)
            output << std::endl;
        if (currCell == 0)
            break;
    }

    output.close();
}

Memory::~Memory()
{
    delete[] data;
}