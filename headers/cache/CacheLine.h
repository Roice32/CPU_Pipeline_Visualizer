#pragma once

#include "Config.h"

template <typename DataType>
class CacheLine
{
public:
    bool validBit;
    address tag;
    DataType data;

    CacheLine<DataType>(): validBit(false) {};
    CacheLine<DataType>(address tag, DataType data): validBit(true), tag(tag), data(data) {}
};