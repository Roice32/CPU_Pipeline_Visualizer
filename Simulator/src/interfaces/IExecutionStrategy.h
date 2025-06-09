#pragma once

#include "CPURegisters.h"
#include "IMemoryAccesser.h"
#include "IClockBoundModule.h"
#include "ExecutionRecorder.h"

class IExecutionStrategy: public IMemoryAccesser
{
protected:
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> fromDEtoMe;
  IClockBoundModule* refToEX;
  std::shared_ptr<CPURegisters> regs;
  std::shared_ptr<ExecutionRecorder> recorder;

  IExecutionStrategy(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> fromDEtoMe,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ExecutionRecorder> recorder) :
      IMemoryAccesser(commPipeWithLS),
      fromDEtoMe(fromDEtoMe),
      refToEX(refToEX),
      regs(registers),
      recorder(recorder) {};

  SynchronizedDataPackage<std::vector<word>> getFinalArgValue(byte src, word param = 0, byte isForZReg = false)
  {
    switch(src)
    {
      case NULL_VAL:
        return std::vector<word> { 0 };
      case IMM:
        return std::vector<word> { param };
      case ADDR:
        {
          SynchronizedDataPackage<std::vector<word>> respFromLS = requestDataAt(param, isForZReg ? WORDS_PER_Z_REGISTER : 1);
          if (!respFromLS.exceptionTriggered)
            return respFromLS;
          return SynchronizedDataPackage<std::vector<word>>(*regs->IP, param, MISALIGNED_ACCESS_HANDL);
        }
      case SP_REG:
        return std::vector<word> { *regs->stackPointer };
      case ST_BASE:
        return std::vector<word> { *regs->stackBase };
      case ST_SIZE:
        return std::vector<word> { *regs->stackSize };
      case R0 ... R7:
        return std::vector<word> { *regs->registers[src - R0] };
      case ADDR_R0 ... ADDR_R7:
        {
          SynchronizedDataPackage<std::vector<word>> respFromLS = requestDataAt(*regs->registers[src - ADDR_R0], isForZReg ? WORDS_PER_Z_REGISTER : 1);
          if (!respFromLS.exceptionTriggered)
            return respFromLS.data;
          return SynchronizedDataPackage<std::vector<word>>(*regs->IP, param, MISALIGNED_ACCESS_HANDL);
        }
      case Z0 ... Z3:
        return *regs->zRegisters[src - Z0];
      default:
        throw std::invalid_argument("Wrong or unimplemented parameter type");
    }
  }

  SynchronizedDataPackage<word> storeResultAtDest(std::vector<word> result, byte destType, word destLocation = 0)
  {
    switch (destType)
    {
      case ADDR:
        {
          SynchronizedDataPackage<std::vector<word>> respFromLS = storeDataAt(destLocation, result.size(), result);
          if (!respFromLS.exceptionTriggered)
            return respFromLS.data.size() > 0 ? respFromLS.data[0] : 0;
          return SynchronizedDataPackage<word>(*regs->IP, destLocation, MISALIGNED_ACCESS_HANDL);
        }
      break;
      case SP_REG:
        *regs->stackPointer = result[0];
        recorder->modifyStackPointer(*regs->stackPointer);
      break;
      case ST_BASE:
        *regs->stackBase = result[0];
        recorder->modifyStackBase(*regs->stackBase);
      break;
      case ST_SIZE:
        *regs->stackSize = result[0];
        recorder->modifyStackSize(*regs->stackSize);
      break;
      case R0 ... R7:
        *regs->registers[destType - R0] = result[0];
        recorder->modifyRRegister(destType - R0, *regs->registers[destType - R0]);
      break;
      case ADDR_R0 ... ADDR_R7:
        {
          SynchronizedDataPackage<std::vector<word>> respFromLS = storeDataAt(*regs->registers[destType - ADDR_R0], result.size(), result);
          if (!respFromLS.exceptionTriggered)
            return respFromLS.data.size() > 0 ? respFromLS.data[0] : 0;
          return SynchronizedDataPackage<word>(*regs->IP, *regs->registers[destType - ADDR_R0], MISALIGNED_ACCESS_HANDL);
        }
      break;
      case Z0 ... Z3:
        *regs->zRegisters[destType - Z0] = result;
        recorder->modifyZRegister(destType - Z0, *regs->zRegisters[destType - Z0]);
      break;
      default:
        throw std::invalid_argument("Wrong or unimplemented destination type");
    }
    return 0;
  }

  SynchronizedDataPackage<std::vector<word>> requestDataAt(address addr, byte howManyWords)
  {
    MemoryAccessRequest newReq(addr, howManyWords);
    SynchronizedDataPackage<MemoryAccessRequest> syncReq(newReq, addr);
    clock_time currTick = refToEX->getCurrTime();
    syncReq.sentAt = currTick;
    fromEXtoLS->sendA(syncReq);
    recorder->pushEXtoLSData(syncReq);
    recorder->cacheEXState();
    recorder->modifyModuleState(EX, "Awaiting "
                                    + std::to_string(howManyWords)
                                    + " words from LS at #"
                                    + convDecToHex(addr));
    refToEX->enterIdlingState();
    while (!fromEXtoLS->pendingB())
      refToEX->returnFromIdlingState();
    SynchronizedDataPackage<std::vector<word>> receivedPckg = fromEXtoLS->getB();
    refToEX->awaitNextTickToHandle(receivedPckg);
    recorder->popPipeData(LStoEX);
    recorder->restoreEXState();
    return receivedPckg;
  }

  SynchronizedDataPackage<std::vector<word>> storeDataAt(address addr, byte howManyWords, std::vector<word> data)
  {
    MemoryAccessRequest newReq(addr, howManyWords, true, data);
    SynchronizedDataPackage<MemoryAccessRequest> syncReq(newReq, addr);
    clock_time currTick = refToEX->getCurrTime();
    syncReq.sentAt = currTick;
    fromEXtoLS->sendA(syncReq);
    recorder->pushEXtoLSData(syncReq);
    std::stringstream statusMsg;
    statusMsg << "Awaiting store of ";
    for (size_t i = 0; i < data.size(); ++i)
    {
      statusMsg << convDecToHex(data[i]) << " ";
    }
    statusMsg << "by LS at #" << convDecToHex(addr);
    recorder->cacheEXState();
    recorder->modifyModuleState(EX, statusMsg.str());
    refToEX->enterIdlingState();
    while (!fromEXtoLS->pendingB())
      refToEX->returnFromIdlingState();
    SynchronizedDataPackage<std::vector<word>> receivedPckg = fromEXtoLS->getB();
    refToEX->awaitNextTickToHandle(receivedPckg);
    recorder->popPipeData(LStoEX);
    recorder->restoreEXState();
    return receivedPckg;
  }

  void moveIP(Instruction instr)
  {
    *regs->IP += WORD_BYTES;
    if (instr.src1 == IMM || instr.src1 == ADDR)
      *regs->IP += WORD_BYTES;
    if (instr.src2 == IMM || instr.src2 == ADDR)
      *regs->IP += WORD_BYTES;
    recorder->modifyIP(*regs->IP);
  }

  void jumpIP(address newIP)
  {
    *regs->IP = newIP;
    recorder->modifyIP(*regs->IP);
  }

public:
  virtual void executeInstruction(SynchronizedDataPackage<Instruction> instrPackage) = 0;

  void handleException(SynchronizedDataPackage<Instruction> faultyInstr)
  {
    recorder->setEXException(faultyInstr);
    if (*regs->flags & EXCEPTION)
    {
      refToEX->endSimulation();
      recorder->modifyModuleState(EX, "Forcefully ended simulation due to double exception");
      return;
    }

    *regs->flags |= EXCEPTION;
    recorder->modifyFlags(*regs->flags);

    recorder->modifyModuleState(EX, "Awaiting exception handler at #"
                                    + convDecToHex(faultyInstr.handlerAddr));

    SynchronizedDataPackage<std::vector<word>> methodAddressPckg = requestDataAt(faultyInstr.handlerAddr, 1);
    word methodAddress = methodAddressPckg.data[0];

    std::vector<word> savedState;
    savedState.push_back(faultyInstr.associatedIP);
    savedState.push_back(*regs->stackPointer);
    savedState.push_back(*regs->flags);
    savedState.push_back(faultyInstr.excpData);
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
      savedState.push_back(*regs->registers[reg]);
    storeDataAt(SAVE_STATE_ADDR, REGISTER_COUNT + 4, savedState);

    std::vector<address> savedStateAddrs;
    for (int ind = 0; ind < savedState.size(); ++ind)
      savedStateAddrs.push_back(SAVE_STATE_ADDR + ind * WORD_BYTES);

    recorder->addExtraInfo(EX, "Saved {IP, SP, FLAGS, R0-7} at memory range [#"
                               + convDecToHex(SAVE_STATE_ADDR)
                               + " - #"
                               + convDecToHex(SAVE_STATE_ADDR + (REGISTER_COUNT + 4) * WORD_BYTES - 1)
                               + "]");

    jumpIP(methodAddress);
    clock_time lastTick = refToEX->waitTillLastTick();
    fromDEtoMe->sendB(methodAddress);
    recorder->pushEXtoDEData(SynchronizedDataPackage<address>(*regs->IP, lastTick));
  }
};