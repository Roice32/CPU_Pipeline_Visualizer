class Memory
{
private:
    unsigned char* data;

public:
    Memory();
    unsigned char getMemoryCell(unsigned short addr);
    void setMemoryCell(unsigned short addr, unsigned char value);
    ~Memory();
};