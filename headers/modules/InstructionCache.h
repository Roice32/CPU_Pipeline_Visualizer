#pragma once

#include "Config.h"
#include "InterThreadCommPipe.h"

class Decode;

class InstructionCache
{
private:
    InterThreadCommPipe<address, fetch_window>* requestsToLS;
    InterThreadCommPipe<address, fetch_window>* requestsFromDE;
    register_16b* flags;

public:
    InstructionCache(InterThreadCommPipe<address, fetch_window>* commPipeWithLS, InterThreadCommPipe<address, fetch_window>* commPipeWithDE, register_16b* flags);
    fetch_window getFetchWindowFromLS(address addr);
    void passForDecode(fetch_window currBatch);
    void run();
};