#pragma once

#include "MemoryAccessRequest.h"
#include "InterThreadCommPipe.h"

#include <memory>

class IMemoryAccesser
{
protected:
    std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, std::vector<word>>> requestsToLS;
    
    IMemoryAccesser(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, std::vector<word>>> commPipeWithLS) : requestsToLS(commPipeWithLS) {};
    virtual std::vector<word> requestDataAt(address addr,  byte howManyWords) = 0;
    virtual void storeDataAt(address addr, byte howManyWords, std::vector<word> data) = 0;
};