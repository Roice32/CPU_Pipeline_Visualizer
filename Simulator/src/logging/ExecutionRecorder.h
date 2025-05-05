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
  void dumpToJSON(const std::string &filename);
};