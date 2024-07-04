#pragma once

#include "IFetchWindowRequester.cpp"

class InstructionCache : public IFetchWindowRequester
{
private:
    register_16b* const IP;
public:
    InstructionCache(LoadStore* lsModuleRef, register_16b* ip);
    fetch_window requestFetchWindow(address addr);
};