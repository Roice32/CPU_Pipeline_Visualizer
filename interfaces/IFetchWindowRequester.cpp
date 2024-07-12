#pragma once

#include "LoadStore.h"

#include <memory>

class IFetchWindowRequester
{
protected:
    std::shared_ptr<LoadStore> const LSModule;

public:
    IFetchWindowRequester(std::shared_ptr<LoadStore> lsModule): LSModule(lsModule) {};
    virtual void requestFetchWindow() = 0;
};