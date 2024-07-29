#pragma once

#include "Config.h"

template <typename DataType>
class CacheLine
{
public:
    bool validBit;
    address tag;
    DataType data;
    clock_time lastHitTime;

    CacheLine<DataType>(): validBit(false), lastHitTime(0) {};
};