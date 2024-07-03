#include "../interfaces/IMemoryHandler.cpp"

class LoadStore : public IMemoryHandler
{
public:
    LoadStore(Memory* simulatedMemory): IMemoryHandler(simulatedMemory) {};
    unsigned char loadFrom(unsigned short addr);
    void storeAt(unsigned short addr, unsigned char value);
};