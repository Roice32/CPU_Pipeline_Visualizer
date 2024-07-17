#pragma once

#include "MemoryAccessRequest.h"
#include "InterThreadCommPipe.h"
#include "SynchronizedDataPackage.h"

#include <memory>

class IMemoryAccesser
{
protected:
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> fromEXtoLS;
    
    IMemoryAccesser(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS) : fromEXtoLS(commPipeWithLS) {};
    virtual std::vector<word> requestDataAt(address addr,  byte howManyWords) = 0;
    virtual void storeDataAt(address addr, byte howManyWords, std::vector<word> data) = 0;
};