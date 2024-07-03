#include "../headers/Memory.h"

class IMemoryHandler
{
protected:
    Memory* target;
public:
    IMemoryHandler(Memory* target): target(target) {};
    
    virtual unsigned char loadFrom(unsigned short addr) = 0;
    virtual unsigned int bufferedLoadFrom(unsigned short addr) = 0;
    virtual void storeAt(unsigned short addr, unsigned char value) = 0;
    
    ~IMemoryHandler()
    {
        delete target;
    }
};