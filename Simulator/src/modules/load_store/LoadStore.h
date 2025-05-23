#pragma once

#include "LSLogger.h"
#include "../../synchronization/MemoryAccessRequest.h"
#include "../../synchronization/InterThreadCommPipe.h"
#include "../../cache/KWayAssociativeCache.h"
#include "../../interfaces/IMemoryHandler.h"
#include "../../interfaces/IClockBoundModule.h"

class LoadStore : public IMemoryHandler, public IClockBoundModule, public LSLogger
{
private:
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> fromICtoMe;
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> fromEXtoMe;
  KWayAssociativeCache<word> cache;
  bool physicalMemoryAccessHappened;

  byte loadFrom(address addr) override;
  fetch_window bufferedLoadFrom(address addr) override;
  void storeAt(address addr, byte value) override;
  std::vector<word> handleRequestFromEX(MemoryAccessRequest req);
  void emptyCacheIntoMemory();

public:
  LoadStore(std::shared_ptr<Memory> simulatedMemory,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithIC,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithEX,
    std::shared_ptr<ClockSyncPackage> clockSyncVars);

  void executeModuleLogic() override;
  void run() override;
};