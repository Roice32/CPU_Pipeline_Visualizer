#pragma once

#include "Config.h"
#include "InterThreadCommPipe.h"

#include <memory>

class InstructionCache
{
private:
    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> requestsToLS;
    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> requestsFromDE;
    std::shared_ptr<register_16b> flags;

public:
    InstructionCache(std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithLS, std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithDE, std::shared_ptr<register_16b> flags);
    fetch_window getFetchWindowFromLS(address addr);
    void passForDecode(fetch_window currBatch);
    void run();
};