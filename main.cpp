#include <iostream>

#include "headers/Memory.h"
#include "CPU.h"

int main(int argc, char** argv)
{
    try
    {
        Memory* mem = new Memory(argv[1]);
        CPU cpu(mem);
        cpu.run();
    }
    catch (int except)
    {
        if (except == 404)
            std::cout << "Could not open specified source file.\n";
    }
    catch (const char* except)
    {
        std::cout << except << "\n";
    }
    return 0;
}
