#pragma once

#include "IFetchWindowRequester.cpp"

class Decode;

class InstructionCache : public IFetchWindowRequester
{
private:
    fetch_window currBatch;
    register_16b* const IP;
    Decode* DEModule;

public:
    InstructionCache(LoadStore* lsModuleRef, register_16b* ip);
    void requestFetchWindow();
    void passForDecode();
    void setDEModule(Decode* deModuleRef);
};