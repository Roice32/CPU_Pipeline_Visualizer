#pragma once

#include "Config.h"

class Memory
{
private:
    byte* data;

    static byte hexCharToDec(const char digit);
    static void jumpToNewAddr(address& currAddr, const byte* newAddr);
    void storeData(address& currAddr, const byte* instr);

public:
    Memory(const char* hexSourceFilePath);
    byte getMemoryCell(address addr);
    void setMemoryCell(address addr, byte value);
    void dumpMemoryContents(char* outputFile);
    ~Memory();
};