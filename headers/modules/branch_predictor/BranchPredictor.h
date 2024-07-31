#pragma once

#include "JumpInstrData.h"
#include "BranchingFeedback.h"

#include <unordered_map>

class BranchPredictor//: public IClockBoundModule, public ILogger
{
private:
    // comm channels
    std::unordered_map<address, JumpInstrData> cache;

public:
    
};