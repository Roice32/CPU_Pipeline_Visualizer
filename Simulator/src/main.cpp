#include "CPU.h"
#include "ExecutionRecorder.h"

int main(int argc, char** argv)
{
  std::shared_ptr<Memory> mem;
  try
  {
    mem = std::make_shared<Memory>(argv[1]);
  } catch (const char* ex)
  {
    std::cout << "Failure: Invalid input file\n";
    return 1;
  }
  
  std::shared_ptr<ExecutionRecorder> recorder = std::make_shared<ExecutionRecorder>(mem);
  
  CPU cpu(mem, recorder);
  cpu.runSimulation();

  if (argc >= 3 && argv[2][0] != '\0')
    recorder->dumpSimulationToJSONs(argv[2]);
  
  return 0;
}