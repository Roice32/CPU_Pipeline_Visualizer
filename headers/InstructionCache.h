#pragma once

#include "IFetchWindowRequester.cpp"
#include "Decode.h"

class InstructionCache : public IFetchWindowRequester
{
private:
    fetch_window currBatch;
    register_16b* const IP;
    Decode* DEModule;

public:
    InstructionCache(LoadStore* lsModuleRef, register_16b* ip);
    void requestFetchWindow(address addr);
    void passForDecode();
};