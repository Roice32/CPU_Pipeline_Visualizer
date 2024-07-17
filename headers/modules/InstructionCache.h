#pragma once

#include "Config.h"
#include "InterThreadCommPipe.h"
#include "SynchronizedDataPackage.h"
#include "IClockBoundModule.cpp"

#include <memory>

class InstructionCache: public IClockBoundModule
{
private:
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> fromMetoLS;
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, bool>> fromMetoDE;
    std::shared_ptr<register_16b> IP;
    register_16b internalIP;

    fetch_window getFetchWindowFromLS(address addr);
    void passForDecode(fetch_window currBatch);
    
public:
    InstructionCache(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithLS,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, bool>> commPipeWithDE,
        std::shared_ptr<ClockSyncPackage> clockSyncVars,
        std::shared_ptr<register_16b> ip);
    
    bool executeModuleLogic() override;
};