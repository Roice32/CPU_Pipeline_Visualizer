#include "Memory.h"
#include "../Config.h"

class CPU
{
private:
    const Memory* memoryUnit;
    register_16b registers[8];
    register_16b IP;
    register_16b flags;
    register_16b stackBase;
    register_16b stackSize;
    register_16b stackPointer;
public:
    
};