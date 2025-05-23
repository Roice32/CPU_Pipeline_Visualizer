#pragma once

#include "../../logging/ExecutionRecorder.h"
#include "../../synchronization/InterThreadCommPipe.h"
#include "../../interfaces/IClockBoundModule.h"
#include "../../cache/DMCache.h"

class InstructionCache: public IClockBoundModule
{
private:
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> fromMetoLS;
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, SynchronizedDataPackage<address>>> fromMetoDE;
  register_16b internalIP;
  DMCache<fetch_window> cache;
  std::shared_ptr<ExecutionRecorder> recorder;

  fetch_window getFetchWindowFromLS(address addr);
  void passForDecode(fetch_window currBatch);
  bool checkIPChangeSignal();
  
public:
  InstructionCache(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, SynchronizedDataPackage<address>>> commPipeWithDE,
    std::shared_ptr<ClockSyncPackage> clockSyncVars,
    std::shared_ptr<register_16b> ip,
    std::shared_ptr<ExecutionRecorder> recorder);
  
  void executeModuleLogic() override;
  void run() override;
};