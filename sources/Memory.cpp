#include <fstream>

#include "Memory.h"
#include "Config.h"

#include <cassert>

byte Memory::hexCharToDec(const char digit)
{
    return digit - (digit >= 'a' ? 'a' - 10 : '0');
}

void Memory::jumpToNewAddr(address& currAddr, const byte* newAddr)
{
    currAddr = (address) 0;
    for (int i = 0; i < ADDRESS_WIDTH / 4; ++i)
    {
        currAddr <<= 4;
        currAddr |= hexCharToDec(newAddr[i]);
    }
}

void Memory::storeData(address& currAddr, const byte* instr)
{
    for (int byteNo = 0; byteNo < WORD_BYTES; ++byteNo)
    {
        byte newData = (hexCharToDec(instr[byteNo * 2]) << 4) | hexCharToDec(instr[byteNo * 2 + 1]);
        this -> setMemoryCell(currAddr++, newData);
    }
}

Memory::Memory(const char* hexSourceFilePath)
{
    address currAddr = 0;
    byte instrLine[6];
    
    std::ifstream sourceCodeFile(hexSourceFilePath);

    assert(sourceCodeFile.is_open() && "Unable to open specified input file");

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
    auto foundMemoryCell = data.find(addr);
    if (foundMemoryCell == data.end())
    {
        data.insert({addr, 0});
        return 0;
    }
    return foundMemoryCell->second;
}

void Memory::setMemoryCell(address addr, byte value)
{
    auto foundMemoryCell = data.find(addr);
    if (foundMemoryCell == data.end())
        data.insert({addr, value});
    else
        foundMemoryCell->second = value;
}

void Memory::dumpMemoryContents(const char* outputFile)
{
    std::ofstream output(outputFile);
    for(auto keyValuePair = data.begin(); keyValuePair != data.end(); ++keyValuePair)
        output << keyValuePair->first << ": " << word (keyValuePair->second) << '\n';
    output.close();
}