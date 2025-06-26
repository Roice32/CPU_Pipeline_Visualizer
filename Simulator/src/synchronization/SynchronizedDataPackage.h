#pragma once

#include "Config.h"

template <typename DataType>
class SynchronizedDataPackage
{
public:
  DataType data;
  clock_time sentAt;
  address associatedIP;

  bool exceptionTriggered;
  word excpData;
  address handlerAddr;

  SynchronizedDataPackage<DataType> ():
    exceptionTriggered(false) {};
  SynchronizedDataPackage<DataType> (DataType data, address associatedIP = 0):
    data(data),
    associatedIP(associatedIP),
    exceptionTriggered(false) {};
  SynchronizedDataPackage<DataType> (address associatedIP, word excpData, address handlerAddr):
    exceptionTriggered(true),
    associatedIP(associatedIP),
    excpData(excpData),
    handlerAddr(handlerAddr) {};
};