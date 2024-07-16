#pragma once

#include "Config.h"
#include "InterThreadCommPipe.h"
#include "IClockBoundModule.cpp"

#include <memory>

class InstructionCache: public IClockBoundModule
{
private:
    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> requestsToLS;
    std::shared_ptr<InterThreadCommPipe<address, fetch_window>> requestsFromDE;
    std::shared_ptr<ClockSyncPackage> clockSyncVars;

    fetch_window getFetchWindowFromLS(address addr);
    void passForDecode(fetch_window currBatch);
    
public:
    InstructionCache(std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithLS,
        std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithDE,
        std::shared_ptr<ClockSyncPackage> clockSyncVars);
    
    bool executeModuleLogic() override;
};