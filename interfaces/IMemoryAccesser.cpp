#pragma once

#include "MemoryAccessRequest.h"
#include "InterThreadCommPipe.h"

#include <memory>

class IMemoryAccesser
{
protected:
    std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> requestsToLS;
    
    IMemoryAccesser(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS) : requestsToLS(commPipeWithLS) {};
    virtual word requestDataAt(word addr) = 0;
    virtual void storeDataAt(word addr, word data) = 0;
};