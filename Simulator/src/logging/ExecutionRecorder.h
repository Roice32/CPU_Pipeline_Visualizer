#pragma once

#include <iostream>
#include <fstream>

#include "ExecutionState.h"

class ExecutionRecorder
{
private:
  std::vector<ExecutionState> states = {};

public:
  ExecutionRecorder(std::shared_ptr<Memory> mem);
  void goToNextState();
  void modifyModuleState(const std::string& moduleName, const std::string& state);
  void dumpToJSON(const std::string &filename);
};