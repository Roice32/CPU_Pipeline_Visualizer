#pragma once

#include "Config.h"

class BranchingFeedback
{
public:
    address jmpInstrAddr;
    address jmpDestAddr;
    bool conditional;
    bool taken;
    bool signalingWrongPrediction;

    BranchingFeedback(address jmpInstrAddr, address jmpDestAddr):
        jmpInstrAddr(jmpInstrAddr), jmpDestAddr(jmpDestAddr), conditional(false), taken(true), signalingWrongPrediction(false) {}
    BranchingFeedback(address jmpInstrAddr, address jmpDestAddr, bool taken):
        jmpInstrAddr(jmpInstrAddr), jmpDestAddr(jmpDestAddr), conditional(true), taken(taken), signalingWrongPrediction(false) {}
        
    BranchingFeedback& signalWrongPrediction()
    {
        signalingWrongPrediction = true;
        return *this;
    }
};