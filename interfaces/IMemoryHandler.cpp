#include "../headers/Memory.h"
#include "../Config.h"

class IMemoryHandler
{
protected:
    Memory* target;
public:
    IMemoryHandler(Memory* target): target(target) {};
    
    virtual byte loadFrom(address addr) = 0;
    virtual fetch_window bufferedLoadFrom(address addr) = 0;
    virtual void storeAt(address addr, byte value) = 0;
    
    ~IMemoryHandler()
    {
        delete target;
    }
};