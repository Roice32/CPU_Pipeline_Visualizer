#pragma once

#include "Config.h"

template <typename DataType>
class SynchronizedDataPackage
{
public:
    DataType data;
    clock_time sentAt;
    address associatedIP;

    SynchronizedDataPackage<DataType> (DataType data, address associatedIP = 0):
         data(data), associatedIP(associatedIP) {};
    SynchronizedDataPackage<DataType> () {};
};