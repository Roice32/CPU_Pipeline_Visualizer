#pragma once

#include "DELogger.h"
#include "InterThreadCommPipe.h"
#include "IClockBoundModule.h"
#include "ExecutionRecorder.h"
#include "WorkTempStorage.h"

class Decode: public IClockBoundModule, public DELogger
{
private:
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, SynchronizedDataPackage<address>>> fromICtoMe;
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> fromMetoEX;
  std::shared_ptr<register_16b> const flags;
  WorkTempStorage fwTempStorage;
  address discardUntilAddr;
  std::shared_ptr<ExecutionRecorder> recorder;

  static byte getExpectedParamCount(byte opCode);
  static char providedVsExpectedArgsCountDif(byte opCode, byte src1, byte src2);
  static bool argumentIsUndefined(byte src);
  static bool argumentsAreIncompatible(byte opCode, byte src1, byte src2);
  Instruction decodeInstructionHeader(word header);
  bool processFetchWindow(fetch_window newBatch);

public:
  Decode(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, SynchronizedDataPackage<address>>> commPipeWithIC,
         std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithEX,
         std::shared_ptr<ClockSyncPackage> clockSyncVars,
         std::shared_ptr<register_16b> flags,
         std::shared_ptr<ExecutionRecorder> recorder);

  void executeModuleLogic() override;
};