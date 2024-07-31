#pragma once

#include "Config.h"

class JumpInstrData
{
public:
    address jmpDestAddr;
    bool conditional;
    word timesTaken;

    JumpInstrData(address jmpDestAddr, bool conditional):
        jmpDestAddr(jmpDestAddr), conditional(conditional), timesTaken(0) {};
    void inc()
        { ++timesTaken; }
    void reset()
        { timesTaken = 0; }
};