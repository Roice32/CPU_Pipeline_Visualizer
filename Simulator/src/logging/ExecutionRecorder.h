#pragma once

#include <iostream>
#include <fstream>
#include <map>

#include "ExecutionState.h"

class ExecutionRecorder
{
private:
  std::map<address, word> memory = {};

  std::vector<ExecutionState> states = {};
  void dumpStateToJSON(const ExecutionState& state, const std::string& outputDirPath);
  void updateMemory(const std::unordered_map<address, word>& memoryChanges);
  void dumpMemoryToJSON(const clock_time cycle, const std::string& outputDirPath);

public:
  ExecutionRecorder(std::shared_ptr<Memory> mem);
  void goToNextState();
  void modifyModuleState(const std::string& moduleName, const std::string& state);
  void dumpSimulationToJSONs(const std::string& outputDirPath);
};