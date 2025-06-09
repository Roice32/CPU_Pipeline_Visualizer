#include "Decode.h"

Decode::Decode(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, SynchronizedDataPackage<address>>> commPipeWithIC,
               std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithEX,
               std::shared_ptr<ClockSyncPackage> clockSyncVars,
               std::shared_ptr<register_16b> flags,
               std::shared_ptr<ExecutionRecorder> recorder):
    IClockBoundModule(clockSyncVars, DE_CYCLES_PER_OP),
    fromICtoMe(commPipeWithIC),
    fromMetoEX(commPipeWithEX),
    flags(flags),
    discardUntilAddr(DUMMY_ADDRESS),
    recorder(recorder) {};

byte Decode::getExpectedParamCount(byte opCode)
{
  if (opCode < JMP || opCode == GATHER || opCode == SCATTER)
    return 2;
  if (opCode == RET || opCode == END_SIM || opCode == EXCP_EXIT)
    return 0;
  return 1;
}

char Decode::providedVsExpectedArgsCountDif(byte opCode, byte src1, byte src2)
{
  byte expectedParamCount = getExpectedParamCount(opCode);
  if (expectedParamCount == 2 && src2 == NULL_VAL)
    return -1;
  if (expectedParamCount == 1 && src1 == NULL_VAL && opCode != POP)
    return -1;
  if (expectedParamCount == 1 && src2 != NULL_VAL)
    return 1;
  if (expectedParamCount == 0 && src1 != NULL_VAL)
    return 1;
  if (src1 == NULL_VAL && src2 != NULL_VAL)
    return expectedParamCount == 0 ? 1 : -1;
  return 0;
}

bool Decode::argumentIsUndefined(byte src)
{
  return !((src >= NULL_VAL && src <= ST_SIZE) ||
    isReg(src) ||
    isZReg(src) || 
    isAddrReg(src));
}

bool Decode::argumentsAreIncompatible(byte opCode, byte src1, byte src2)
{
  bool param1IsStack = src1 >= SP_REG && src1 <= ST_SIZE;
  bool param2IsStack = src2 >= SP_REG && src2 <= ST_SIZE;
  bool param1IsZReg = isZReg(src1);
  bool param2IsZReg = isZReg(src2);
  
  bool immGivenAsDestination = src1 == IMM && 
    (opCode < MUL || opCode == POP);
  bool twoStackSrcsForMov = opCode == MOV && 
    param1IsStack && param2IsStack;
  bool stackSrcAnywhereOtherThanMovOrCmp = opCode != MOV && 
    opCode != CMP && (param1IsStack || param2IsStack);
  bool zRegWithForbiddenOp = (param1IsZReg || param2IsZReg) && 
    !(opCode >= ADD && opCode <= DIV || opCode == GATHER || opCode == SCATTER);
  bool forbiddenOtherParameterForZReg = (param1IsZReg && src2 != ADDR && !isAddrReg(src2) && !param2IsZReg) ||
    (param2IsZReg && src1 != ADDR && !isAddrReg(src1) && !param1IsZReg);
  bool notTwoZRegsForGatherScatter = (opCode == GATHER || opCode == SCATTER) &&
    !(param1IsZReg && param2IsZReg);

  return immGivenAsDestination || 
    twoStackSrcsForMov || 
    stackSrcAnywhereOtherThanMovOrCmp || 
    zRegWithForbiddenOp || 
    forbiddenOtherParameterForZReg ||
    notTwoZRegsForGatherScatter;
}

Instruction Decode::decodeInstructionHeader(word instruction)
{
  byte opCode = instruction >> 10;
  if (opCode == UNINITIALIZED_MEM)
    return Instruction(UNINITIALIZED_MEM);
  if (opCode == UNDEFINED || opCode > EXCP_EXIT && (opCode != GATHER && opCode != SCATTER))
    return Instruction(UNDEFINED);
  if (opCode == EXCP_EXIT && !(*flags & EXCEPTION))
    return Instruction(UNDEFINED);

  byte src1 = (instruction >> 5) & 0b11111;
  byte src2 = instruction & 0b11111;

  return Instruction(opCode, src1, src2);
}

bool Decode::processFetchWindow(fetch_window newBatch)
{
  Instruction instr = decodeInstructionHeader(word (newBatch >> ((FETCH_WINDOW_BYTES - WORD_BYTES) * 8)));
  SynchronizedDataPackage<Instruction> syncResponse(instr, fwTempStorage.getAssociatedInstrAddr());
  
  if (instr.opCode == UNDEFINED)
  {
    syncResponse.exceptionTriggered = true;
    syncResponse.excpData = UNKNOWN_OP_CODE;
    syncResponse.handlerAddr = INVALID_DECODE_HANDL;
  }
  
  char argsCountDif = providedVsExpectedArgsCountDif(instr.opCode, instr.src1, instr.src2);
  if (argsCountDif != 0)
  {
    syncResponse.exceptionTriggered = true;
    syncResponse.excpData = argsCountDif < 0 ? NULL_SRC : NON_NULL_SRC;
    syncResponse.handlerAddr = INVALID_DECODE_HANDL;
  }

  if (argumentIsUndefined(instr.src1) || argumentIsUndefined(instr.src2) || argumentsAreIncompatible(instr.opCode, instr.src1, instr.src2))
  {
    syncResponse.exceptionTriggered = true;
    syncResponse.excpData = INCOMPATIBLE_PARAMS;
    syncResponse.handlerAddr = INVALID_DECODE_HANDL;
  }

  byte paramsCount = 0;
  if (instr.opCode != UNINITIALIZED_MEM && !syncResponse.exceptionTriggered)
  {
    if (instr.src1 == IMM || instr.src1 == ADDR)
    {
      syncResponse.data.param1 = newBatch >> ((FETCH_WINDOW_BYTES - WORD_BYTES * 2) * 8);
      ++paramsCount;
    }
    if (instr.src2 == IMM || instr.src2 == ADDR)
    {
      syncResponse.data.param2 = newBatch >> ((FETCH_WINDOW_BYTES - WORD_BYTES * (paramsCount == 0 ? 2 : 3)) * 8);
      ++paramsCount;
    }
  }
  fwTempStorage.shiftUsedWords(paramsCount + 1);
  waitTillLastTick();
  syncResponse.sentAt = clockSyncVars->cycleCount;
  fromMetoEX->sendA(syncResponse);
  recorder->pushDEtoEXData(syncResponse);
  recorder->lastDecodedInstruction("#" + convDecToHex(syncResponse.associatedIP) + ": "
                                   + instr.toString());
  recorder->rewriteDEWorkTempStorage(fwTempStorage.storedFWs,
                                     fwTempStorage.cacheStartAddr,
                                     fwTempStorage.storedWordsCount);
  return true;
}

void Decode::executeModuleLogic()
{
  if (fromMetoEX->pendingB())
  {
    SynchronizedDataPackage<address> ipChangePckg = fromMetoEX->getB();
    recorder->popPipeData(EXtoDE);
    awaitNextTickToHandle(ipChangePckg);
    recorder->modifyModuleState(DE, "Acknowledged IP change from EX to #" + convDecToHex(ipChangePckg.data));
    clock_time currTick = getCurrTime();
    SynchronizedDataPackage<address> mssgToIC(ipChangePckg.data);
    mssgToIC.sentAt = currTick;
    fromICtoMe->sendB(mssgToIC);
    recorder->pushDEtoICData(mssgToIC);
    discardUntilAddr = ipChangePckg.data;
    if (discardUntilAddr % 2 == 1)
    {
      recorder->modifyModuleState(DE,
        "Sending misaligned IP #" + convDecToHex(discardUntilAddr) + " exception to EX");
      SynchronizedDataPackage<Instruction> misalignedIPInstr(discardUntilAddr,
                                                             MISALIGNED_IP,
                                                             MISALIGNED_IP_HANDL);
      fromMetoEX->sendA(misalignedIPInstr);
      recorder->pushDEtoEXData(misalignedIPInstr);
      discardUntilAddr = DUMMY_ADDRESS;

      return;
    }

    fwTempStorage.discardCurrent();
    recorder->rewriteDEWorkTempStorage(fwTempStorage.storedFWs,
                                       fwTempStorage.cacheStartAddr,
                                       fwTempStorage.storedWordsCount);
    recorder->addExtraInfo(DE,
      "Discarded work temp storage due to IP change to #" + convDecToHex(ipChangePckg.data));
    while (fromICtoMe->pendingA() && clockSyncVars->running)
      recorder->addExtraInfo(DE,
        "Discarded fetch window at address #" + convDecToHex(fromICtoMe->getA().associatedIP) + " from IC");
      recorder->popPipeData(ICtoDE);
  }

  while (fromICtoMe->pendingA() && discardUntilAddr != DUMMY_ADDRESS && clockSyncVars->running)
  {
    SynchronizedDataPackage<fetch_window> nextBatch = fromICtoMe->getA();
    recorder->popPipeData(ICtoDE);
    awaitNextTickToHandle(nextBatch);
    if (nextBatch.associatedIP == discardUntilAddr / FETCH_WINDOW_BYTES * FETCH_WINDOW_BYTES)
    {
      fwTempStorage.overwriteCache(nextBatch.data, nextBatch.associatedIP);
      fwTempStorage.shiftUsedWords((discardUntilAddr - nextBatch.associatedIP) / WORD_BYTES);
      discardUntilAddr = DUMMY_ADDRESS;
      recorder->rewriteDEWorkTempStorage(fwTempStorage.storedFWs,
                                         fwTempStorage.cacheStartAddr,
                                         fwTempStorage.storedWordsCount);
    }
    else
      recorder->addExtraInfo(DE,
        "Discarded fetch window at address #" + convDecToHex(nextBatch.associatedIP) + " from IC");
  }

  if (discardUntilAddr != DUMMY_ADDRESS)
    return;

  if (fwTempStorage.canProvideFullInstruction())
  {
    recorder->modifyModuleState(DE,
      "Decoding instruction at address #" + convDecToHex(fwTempStorage.getAssociatedInstrAddr()));
    processFetchWindow(fwTempStorage.getFullInstrFetchWindow());
    return;
  }

  if (fromICtoMe->pendingA())
  {
    SynchronizedDataPackage<fetch_window> receivedFW = fromICtoMe->getA();
    recorder->popPipeData(ICtoDE);
    awaitNextTickToHandle(receivedFW);
    if (fwTempStorage.getStoredWordsCount() == 0)
      fwTempStorage.overwriteCache(receivedFW.data, receivedFW.associatedIP);
    else
      fwTempStorage.concatNewFW(receivedFW.data);
    recorder->rewriteDEWorkTempStorage(fwTempStorage.storedFWs,
                                       fwTempStorage.cacheStartAddr,
                                       fwTempStorage.storedWordsCount);
    recorder->modifyModuleState(DE,
      "Decoding instruction at address #" + convDecToHex(fwTempStorage.getAssociatedInstrAddr()));
    processFetchWindow(fwTempStorage.getFullInstrFetchWindow());
  }
}