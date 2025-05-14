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

void ExecutionRecorder::modifyModuleState(const std::string& moduleName, const std::string& state)
{
  if (moduleName == "IC")
    states.back().IC.state = state;
  else if (moduleName == "LS")
    states.back().LS.state = state;
  else if (moduleName == "DE")
    states.back().DE.state = state;
  else if (moduleName == "EX")
    states.back().EX.state = state;
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
