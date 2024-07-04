#include "../Config.h"

class Memory
{
private:
    byte* data;

public:
    Memory(char* hexSourceFilePath);
    byte getMemoryCell(address addr);
    void setMemoryCell(address addr, byte value);
    ~Memory();
};