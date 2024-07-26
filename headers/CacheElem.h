#pragma once

#include "Config.h"

template <typename DataType>
class CacheElement
{
public:
    DataType data;
    clock_time lastHitTime;

    CacheElement<DataType> (DataType data, clock_time lastHitTime):
        data(data), lastHitTime(lastHitTime) {};
    void updateLastHitTime(clock_time newHitTime)
    {
        lastHitTime = newHitTime;
    }
};