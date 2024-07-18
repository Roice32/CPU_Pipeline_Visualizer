#pragma once

#include "Instruction.h"

class LoggablePackage
{
public:
    Instruction instr;
    word actualParam1;
    word actualParam2;
    bool newLine;
    address ip;
    fetch_window fetchWindow;

    LoggablePackage(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true):
        instr(instr), actualParam1(actualParam1), actualParam2(actualParam2), newLine(newLine) {};

    LoggablePackage(address ip, fetch_window fetchWindow):
        ip(ip), fetchWindow(fetchWindow) {};
};