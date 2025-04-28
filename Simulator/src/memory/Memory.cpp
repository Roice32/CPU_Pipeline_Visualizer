#include <fstream>

#include "Memory.h"
#include "Config.h"

#include <cassert>

byte Memory::hexCharToDec(const char digit)
{
    return digit - (digit >= 'a' ? 'a' - 10 : '0');
}

void Memory::jumpToNewAddr(address& currAddr, const char* newAddr)
{
    currAddr = (address) 0;
    for (int i = 0; i < ADDRESS_WIDTH / 4; ++i)
    {
        currAddr <<= 4;
        currAddr |= hexCharToDec(newAddr[i]);
    }
}

void Memory::storeData(address& currAddr, const char* instr)
{
    for (int byteNo = 0; byteNo < WORD_BYTES; ++byteNo)
    {
        byte newData = (hexCharToDec(instr[byteNo * 2]) << 4) | hexCharToDec(instr[byteNo * 2 + 1]);
        this -> setMemoryCell(currAddr++, newData);
    }
}

bool Memory::isValidInputLine(std::string inLine)
{
    if (inLine.size() > 5)
        return false;
    byte ind = 0;
    if (inLine.size() == 5)
    {
        if (inLine[0] != '#')
            return false;
        ind = 1;
    }
    while (ind < inLine.size())
    {
        if (!(inLine[ind] >= '0' && inLine[ind] <= '9' || inLine[ind] >= 'a' && inLine[ind] <= 'f'))
            return false;
        ++ind;
    }

    return true;
}

Memory::Memory(const char* hexSourceFilePath)
{
    address currAddr = 0;
    std::string instrString = "";
    
    std::ifstream sourceCodeFile(hexSourceFilePath);

    assert(sourceCodeFile.is_open() && "Unable to open specified input file");

    while(sourceCodeFile >> instrString)
    {
        if (!isValidInputLine(instrString))
            throw "INVALID_INPUT_FILE";

        if (char (instrString.c_str()[0]) == '#')
            jumpToNewAddr(currAddr, instrString.c_str() + 1);
        else
            storeData(currAddr, instrString.c_str());
    }

    sourceCodeFile.close();
}

byte Memory::getMemoryCell(address addr)
{
    auto foundMemoryCell = data.find(addr);
    if (foundMemoryCell == data.end())
        return 0;
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