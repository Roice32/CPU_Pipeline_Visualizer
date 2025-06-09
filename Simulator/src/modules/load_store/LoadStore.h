#pragma once

#include "MemoryAccessRequest.h"
#include "InterThreadCommPipe.h"
#include "KWayAssociativeCache.h"
#include "IMemoryHandler.h"
#include "IClockBoundModule.h"
#include "ExecutionRecorder.h"

class LoadStore : public IMemoryHandler, public IClockBoundModule
{
private:
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> fromICtoMe;
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> fromEXtoMe;
  KWayAssociativeCache<word> cache;
  bool physicalMemoryAccessHappened;
  std::shared_ptr<ExecutionRecorder> recorder;

  byte loadFrom(address addr) override;
  fetch_window bufferedLoadFrom(address addr) override;
  void storeAt(address addr, byte value) override;
  std::vector<word> handleRequestFromEX(MemoryAccessRequest req);

public:
  LoadStore(std::shared_ptr<Memory> simulatedMemory,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<address>, SynchronizedDataPackage<fetch_window>>> commPipeWithIC,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithEX,
    std::shared_ptr<ClockSyncPackage> clockSyncVars,
    std::shared_ptr<ExecutionRecorder> recorder);

  void executeModuleLogic() override;
};