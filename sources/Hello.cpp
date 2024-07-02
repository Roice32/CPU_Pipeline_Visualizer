#include <iostream>
#include "../headers/Hello.h"

Hello::Hello(int count)
{
    while (count > 0)
    {
        std::cout << "Hi\n";
        --count;
    }
}

void fnc() {
    std::cout << "000" << std::endl;
}