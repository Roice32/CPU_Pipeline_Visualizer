#include "headers/Memory.h"
#include "CPU.h"

int main(int argc, char** argv)
{
    std::shared_ptr<Memory> mem = std::make_shared<Memory>(argv[1]);
    CPU cpu(mem);
    cpu.runSimulation();
    return 0;
}