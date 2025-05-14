#include "cpu/CPU.h"
#include "logging/ExecutionRecorder.h"

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

  if (argc >= 3 && argv[2][0] != '\0')
    ILogger::openDumpFile(argv[2]);
  else
    ILogger::markOutputForTerminal();
  
  std::shared_ptr<ExecutionRecorder> recorder = std::make_shared<ExecutionRecorder>(mem);
  
  CPU cpu(mem, recorder);
  cpu.runSimulation();
  
  if (argc >= 4 && argv[3][0] != '\0')
    recorder->dumpSimulationToJSONs(argv[3]);
  
  return 0;
}