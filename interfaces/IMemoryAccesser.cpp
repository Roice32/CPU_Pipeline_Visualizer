#pragma once

#include "MemoryAccessRequest.h"
#include "InterThreadCommPipe.h"

class IMemoryAccesser
{
protected:
    InterThreadCommPipe<MemoryAccessRequest, word>* requestsToLS;
    
    IMemoryAccesser(InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithLS) : requestsToLS(commPipeWithLS) {};
    virtual word requestDataAt(word addr) = 0;
    virtual void storeDataAt(word addr, word data) = 0;
};