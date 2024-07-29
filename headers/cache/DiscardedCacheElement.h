#pragma once

#include "Config.h"
template <typename DataType>
class DiscardedCacheElement
{
public:
    DataType data;
    address addr;
    bool discardHappened;

    DiscardedCacheElement<DataType>(): discardHappened(false) {};
    DiscardedCacheElement<DataType>(DataType data, address addr): data(data), addr(addr) {};
};