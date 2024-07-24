#pragma once

#include "ICLogger.h"
#include "InterThreadCommPipe.h"
#include "IClockBoundModule.cpp"

class InstructionCache: public IClockBoundModule, public ICLogger
{
private:
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> fromMetoLS;
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, address>> fromMetoDE;
    register_16b internalIP;

    fetch_window getFetchWindowFromLS(address addr);
    void passForDecode(fetch_window currBatch);
    
public:
    InstructionCache(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithLS,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, address>> commPipeWithDE,
        std::shared_ptr<ClockSyncPackage> clockSyncVars,
        std::shared_ptr<register_16b> ip);
    
    bool executeModuleLogic() override;
    void run() override;
};