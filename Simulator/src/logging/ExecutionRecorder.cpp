#include "ExecutionRecorder.h"

ExecutionRecorder::ExecutionRecorder(std::shared_ptr<Memory> mem)
{
  states.push_back(ExecutionState());
  for (const auto [addr, value] : mem->getMemory())
  {
    states[0].memory[addr] = value;
  }
}

void ExecutionRecorder::dumpToJSON(const std::string &filename)
{
  std::ofstream file(filename);
  if (!file.is_open())
  {
    std::cerr << "Error: Unable to open file " << filename << std::endl;
    return;
  }
  file << "[\n";
  for (size_t i = 0; i < states.size(); ++i)
  {
    file << states[i].toJSON();
    if (i < states.size() - 1)
      file << ",";
    file << "\n";
  }
  file << "]\n";
  file.close();
}