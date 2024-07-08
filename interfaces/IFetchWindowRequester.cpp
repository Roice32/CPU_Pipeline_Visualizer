#pragma once

#include "LoadStore.h"

class IFetchWindowRequester
{
protected:
    LoadStore* const LSModule;

public:
    IFetchWindowRequester(LoadStore* lsModule): LSModule(lsModule) {};
    virtual void requestFetchWindow() = 0;
};