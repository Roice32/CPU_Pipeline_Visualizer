#pragma once

#include "Instruction.h"
#include <vector>

class LoggablePackage
{
public:
    Instruction instr;
    word actualParam1;
    word actualParam2;
    bool newLine;
    address ip;
    fetch_window fetchWindow;
    std::vector<word> data;
    bool wasForEX;
    bool wasStoreOp;

    LoggablePackage(Instruction instr, word actualParam1 = 0, word actualParam2 = 0, bool newLine = true):
        instr(instr), actualParam1(actualParam1), actualParam2(actualParam2), newLine(newLine) {};

    LoggablePackage(address ip, fetch_window fetchWindow):
        ip(ip), fetchWindow(fetchWindow) {};

    LoggablePackage(address addr, Instruction instr):
        ip(addr), instr(instr) {};

    LoggablePackage(std::vector<word> data, address addr, bool wasStoreOp, bool wasForEX):
        data(data), ip(addr), wasStoreOp(wasStoreOp), wasForEX(wasForEX) {};

    LoggablePackage(fetch_window fw, address addr):
        fetchWindow(fw), ip(addr), wasForEX(false), wasStoreOp(false) {};
};