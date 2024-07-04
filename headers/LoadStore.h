#pragma once

#include "IMemoryHandler.cpp"

class LoadStore : public IMemoryHandler
{
public:
    LoadStore(Memory* simulatedMemory): IMemoryHandler(simulatedMemory) {};
    byte loadFrom(address addr);
    fetch_window bufferedLoadFrom(address addr);
    void storeAt(address addr, byte value);
};