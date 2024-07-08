#pragma once

#include "LoadStore.h"

class IMemoryAccesser
{
protected:
    LoadStore* LSModule;
    
    IMemoryAccesser(LoadStore* lsModule) : LSModule(lsModule) {};
    virtual word requestDataAt(word addr) = 0;
    virtual void storeDataAt(word addr, word data) = 0;
};