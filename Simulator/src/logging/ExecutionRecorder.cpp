#include <filesystem>
#include <fstream>
#include <iostream>

#include "ExecutionRecorder.h"

ExecutionRecorder::ExecutionRecorder(std::shared_ptr<Memory> mem, bool singleStateMode)
  : singleStateMode(singleStateMode)
{
  states.push_back(ExecutionState());

  memory = {};
  for (const auto [addr, value] : mem->getMemory())
  {
    states.back().memoryChanges[addr] = value;
  }
}

void ExecutionRecorder::goToNextState()
{
  if (singleStateMode)
  {
    states.back().cycle++;
    states.back().LS.extra = "";
    states.back().IC.extra = "";
    states.back().DE.extra = "";
    states.back().EX.extra = "";
    return;
  }
  ExecutionState newState = states.back();
  newState.cycle++;
  newState.memoryChanges = {};
  newState.LS.extra = "";
  newState.IC.extra = "";
  newState.DE.extra = "";
  newState.EX.extra = "";
  states.push_back(newState);
}

void ExecutionRecorder::modifyModuleState(const Modules& moduleName, const std::string& state)
{
  switch (moduleName)
  {
    case IC:
      states.back().IC.state = state;
      break;
    case LS:
      states.back().LS.state = state;
      break;
    case DE:
      states.back().DE.state = state;
      break;
    case EX:
      states.back().EX.state = state;
      break;
    default:
      std::cerr << "Error: Unknown module name " << moduleName << std::endl;
      break;
  }
}

void ExecutionRecorder::addExtraInfo(const Modules &moduleName, const std::string &extraInfo)
{
  switch (moduleName)
  {
    case IC:
      states.back().IC.extra += extraInfo + "\\n";
      break;
    case LS:
      states.back().LS.extra += extraInfo + "\\n";
      break;
    case DE:
      states.back().DE.extra += extraInfo + "\\n";
      break;
    case EX:
      states.back().EX.extra += extraInfo + "\\n";
      break;
    default:
      std::cerr << "Error: Unknown module name " << moduleName << std::endl;
      break;
  }
}

void ExecutionRecorder::pushToStack(const std::vector<word> &values, bool reverse)
{
  auto& stack = states.back().stack;
  if (reverse)
  {
    for (int i = values.size() - 1; i >= 0; --i)
    {
      stack.push_back(values[i]);
    }
  }
  else
  {
    for (const auto& value : values)
    {
      stack.push_back(value);
    }
  }
}

void ExecutionRecorder::popFromStack(byte count)
{
  std::deque<word>& stack = states.back().stack;
  while (!stack.empty() && count > 0)
  {
    stack.pop_back();
    --count;
  }
}

void ExecutionRecorder::popPipeData(const Pipes &pipeName)
{
  auto& pipes = states.back().pipes;

  switch (pipeName)
  {
    case ICtoLS:
      if (pipes.ICtoLS.empty())
        return;
      pipes.ICtoLS.pop_front();
      break;
    case LStoIC:
      if (pipes.LStoIC.empty())
        return;
      pipes.LStoIC.pop_front();
      break;
    case ICtoDE:
      if (pipes.ICtoDE.empty())
        return;
      pipes.ICtoDE.pop_front();
      break;
    case DEtoIC:
      if (pipes.DEtoIC.empty())
        return;
      pipes.DEtoIC.pop_front();
      break;
    case DEtoEX:
      if (pipes.DEtoEX.empty())
        return;
      pipes.DEtoEX.pop_front();
      break;
    case EXtoDE:
      if (pipes.EXtoDE.empty())
        return;
      pipes.EXtoDE.pop_front();
      break;
    case EXtoLS:
      if (pipes.EXtoLS.empty())
        return;
      pipes.EXtoLS.pop_front();
      break;
    case LStoEX:
      if (pipes.LStoEX.empty())
        return;
      pipes.LStoEX.pop_front();
      break;
    default:
      std::cerr << "Error: Unknown pipe name " << pipeName << std::endl;
      break;
  }
}

void ExecutionRecorder::recordMemoryChanges(const std::vector<address>& addr, const std::vector<word>& value)
{
  auto& lastState = states.back();
  if (lastState.memoryUnchangedSinceCycle != lastState.cycle)
  {
    lastState.memoryUnchangedSinceCycle = lastState.cycle;
  }

  for (size_t i = 0; i < addr.size(); ++i)
  {
    lastState.memoryChanges[addr[i]] = value[i] >> 8;
    lastState.memoryChanges[addr[i] + 1] = value[i] & 0xFF;
  }
}

void ExecutionRecorder::storeLSCacheLine(const byte& tag,
                                         const byte &index,
                                         const byte &innerIndex,
                                         const word &data,
                                         const clock_time &time)
{
  auto& cacheLine = states.back().LS.cache.storage[index].storedLines[innerIndex];
  cacheLine.data = data;
  cacheLine.lastHitTime = time;
  cacheLine.valid = true;
  cacheLine.modified = false;
  cacheLine.tag = tag;
}

void ExecutionRecorder::invalidateICCacheLine(const byte &index)
{
  states.back().IC.cache.storage[index].valid = false;
}

void ExecutionRecorder::swapICCacheLine(const CacheLine<fetch_window> &newLine, const byte &index)
{
  states.back().IC.cache.storage[index].data = newLine.data;
  states.back().IC.cache.storage[index].tag = newLine.tag;
  states.back().IC.cache.storage[index].lastHitTime = newLine.lastHitTime; 
  states.back().IC.cache.storage[index].valid = newLine.valid;
}

void ExecutionRecorder::rewriteDEWorkTempStorage(const fetch_window* fws,
                                                 const address& cacheStartAddr,
                                                 const byte& storedWordsCount)
{
  for (size_t i = 0; i < DE_WORK_MEMORY_FW_SIZE; ++i)
  {
    states.back().DE.fwTempStorage.storedFWs[i] = fws[i];
  }
  states.back().DE.fwTempStorage.cacheStartAddr = cacheStartAddr;
  states.back().DE.fwTempStorage.storedWordsCount = storedWordsCount;
}

void ExecutionRecorder::modifyZRegister(const byte ind, const std::vector<word> &values)
{
  auto& zReg = states.back().registers.Z[ind];
  for (size_t i = 0; i < WORDS_PER_Z_REGISTER && i < values.size(); ++i)
  {
    zReg[i] = values[i];
  }
}

void ExecutionRecorder::setEXException(const SynchronizedDataPackage<Instruction> &faultyInstr)
{
  std::stringstream exceptionMsg;
  switch (faultyInstr.handlerAddr)
  {
    case DIV_BY_ZERO_HANDL:
      exceptionMsg << "Division by 0";
      break;
    case INVALID_DECODE_HANDL:
      switch (faultyInstr.excpData)
      {
        case UNKNOWN_OP_CODE:
          exceptionMsg << "Unknown operation code";
          break;
        case NULL_SRC:
          exceptionMsg << "Null where argument expected";
          break;
        case NON_NULL_SRC:
          exceptionMsg << "Argument where null expected";
          break;
        case INCOMPATIBLE_PARAMS:
          exceptionMsg << "Incompatible parameters (mutually / for given operation)";
          break;
        default:
          exceptionMsg << "Invalid decode handler address";
      }
      break;
    case MISALIGNED_ACCESS_HANDL:
      exceptionMsg << "Request to memory address not aligned to 16b";
      break;
    case STACK_OVERFLOW_HANDL:
      if (faultyInstr.excpData == PUSH_OVERFLOW)
        exceptionMsg << "Over-pushed stack exceeded upper limit";
      else
        exceptionMsg << "Over-popped stack exceeded lower limit";
      break;
    case MISALIGNED_IP_HANDL:
      exceptionMsg << "IP not aligned to 16b";
      break;
    default:
      exceptionMsg << "Unknown exception handler address";
  }
  
  exceptionMsg << " at #" + convDecToHex(faultyInstr.associatedIP);
  states.back().EX.activeException = exceptionMsg.str();
}

void ExecutionRecorder::clearEXException()
{
  states.back().EX.activeException = "";
}

void ExecutionRecorder::dumpSimulationToJSONs(const std::string &outputDirPath)
{
  for (auto& state: states) 
  {
    if (!state.memoryChanges.empty())
    {
      state.memoryUnchangedSinceCycle = state.cycle;
      updateMemory(state.memoryChanges);
      dumpMemoryToJSON(state.cycle, outputDirPath + "/memory/");
    }
    dumpStateToJSON(state, outputDirPath + "/cpu_states/");
  }
}

void ExecutionRecorder::dumpStateToJSON(ExecutionState& state, const std::string& outputDirPath)
{
  std::string stateJsonFilePath = outputDirPath + std::to_string(state.cycle) + ".json";
  std::ofstream file(stateJsonFilePath);
  if (!file.is_open())
  {
    std::cerr << "Error: Unable to create file " << stateJsonFilePath << std::endl;
    return;
  }
  file << state.toJSON();
  file.close();
}

void ExecutionRecorder::updateMemory(const std::unordered_map<address, word>& memoryChanges)
{
  for (const auto& [addr, value] : memoryChanges)
  {
    memory[addr] = value;
  }
}

void ExecutionRecorder::dumpMemoryToJSON(const clock_time cycle, const std::string &outputDirPath)
{
  std::string memoryJsonFilePath = outputDirPath + std::to_string(cycle) + ".json";
  std::ofstream file(memoryJsonFilePath);
  if (!file.is_open())
  {
    std::cerr << "Error: Unable to create file " << memoryJsonFilePath << std::endl;
    return;
  }
  file << "{";
  for (const auto& [addr, value]: memory)
  {
    file << "\"" << std::hex << addr << "\": \"" << value << "\"";
    if (addr != memory.rbegin()->first)
      file << ",";
  }
  file << "}";
  file.close();
}
