#pragma once

#include "CacheLine.h"

template <typename DataType>
class TwoWayCacheSet
{
public:
    CacheLine<DataType> first;
    CacheLine<DataType> second;

    TwoWayCacheSet<DataType>(): first(CacheLine<DataType>()), second(CacheLine<DataType>()) {}
};