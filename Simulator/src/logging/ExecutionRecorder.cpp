#include "ExecutionRecorder.h"

ExecutionRecorder::ExecutionRecorder(std::shared_ptr<Memory> mem)
{
  states.push_back(ExecutionState());
  for (const auto [addr, value] : mem->getMemory())
  {
    states[0].memory[addr] = value;
  }
}

void ExecutionRecorder::goToNextState()
{
  ExecutionState newState = states.back();
  newState.cycle++;
  states.push_back(newState);
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

void ExecutionRecorder::dumpToJSON(const std::string &filename)
{
  std::ofstream file(filename);
  if (!file.is_open())
  {
    std::cerr << "Error: Unable to open file " << filename << std::endl;
    return;
  }
  file << "[";
  for (size_t i = 0; i < states.size(); ++i)
  {
    file << states[i].toJSON();
    if (i < states.size() - 1)
      file << ",";
  }
  file << "]";
  file.close();
}