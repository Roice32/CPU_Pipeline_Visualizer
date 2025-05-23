#include <filesystem>
#include <fstream>
#include <iostream>

#include "ExecutionRecorder.h"

ExecutionRecorder::ExecutionRecorder(std::shared_ptr<Memory> mem)
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
  ExecutionState newState = states.back();
  newState.cycle++;
  states.push_back(newState);
  states.back().memoryChanges = {};
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

void ExecutionRecorder::invalidateICCacheLine(const byte &index)
{
  states.back().IC.cache.storage[index].valid = false;
}

void ExecutionRecorder::swapICCacheLine(const byte &index, const fetch_window &newValue)
{
  states.back().IC.cache.storage[index].data = newValue;
  states.back().IC.cache.storage[index].valid = true;
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

void ExecutionRecorder::dumpStateToJSON(const ExecutionState& state, const std::string& outputDirPath)
{
  std::string stateJsonFilePath = outputDirPath + std::to_string(state.cycle) + ".json";
  std::ofstream file(stateJsonFilePath);
  if (!file.is_open())
  {
    std::cerr << "Error: Unable to open file " << stateJsonFilePath << std::endl;
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
    std::cerr << "Error: Unable to open file " << memoryJsonFilePath << std::endl;
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
