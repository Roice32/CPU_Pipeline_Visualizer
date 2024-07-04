#pragma once

#include "Config.h"

class Memory
{
private:
    byte* data;

public:
    Memory(const char* hexSourceFilePath);
    byte getMemoryCell(address addr);
    void setMemoryCell(address addr, byte value);
    ~Memory();
};