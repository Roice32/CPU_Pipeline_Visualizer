#include "headers/Memory.h"
#include "CPU.h"

int main(int argc, char** argv)
{
    std::shared_ptr<Memory> mem = std::make_shared<Memory>(argv[1]);
    if (argc >= 3 && argv[2][0] != '\0')
        ILogger::openDumpFile(argv[2]);
    else
        ILogger::markOutputForTerminal();
    CPU cpu(mem);
    cpu.runSimulation();
    return 0;
}