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

/*#include "InterThreadCommPipe.h"
#include "thread"

InterThreadCommPipe<const char*, const char*>* pp = new InterThreadCommPipe<const char*, const char*>();

void run1()
{
    pp->sendRequest("ping");
    while(!pp->pendingResponse()) ;
    std::cout << pp->getResponse();
}

void run2()
{
    while (!pp->pendingRequest()) ;
    const char* req = pp->getRequest();
    if (req[1]=='i')
        pp->sendResponse("pong");
    else
        pp->sendResponse("what");
}

int main()
{
    std::thread t1(run1);
    std::thread t2(run2);

    t1.join();
    t2.join();

    return 0;
}*/