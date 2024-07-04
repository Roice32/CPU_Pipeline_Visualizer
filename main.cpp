#include <iostream>

#include "Memory.h"

int main(int, char**)
{
    try
    {
        Memory mem("../program_files/basicTest.hex");
        mem.dumpMemoryContents("../program_files/dump.txt");
    }
    catch (int except)
    {
        if (except == 404)
            std::cout << "Could not open specified source file.\n";
    }
    return 0;
}
