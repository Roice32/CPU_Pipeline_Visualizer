#pragma once

#include "Config.h"
#include "InterThreadCommPipe.h"

class Decode;

class InstructionCache
{
private:
    InterThreadCommPipe<address, fetch_window>* requestsToLS;
    fetch_window currBatch;
    register_16b* IP;
    Decode* DEModule;

public:
    InstructionCache(InterThreadCommPipe<address, fetch_window>* commPipeWithLS, register_16b* ip);
    void requestFetchWindow();
    void passForDecode();
    void setDEModule(Decode* deModuleRef);
};