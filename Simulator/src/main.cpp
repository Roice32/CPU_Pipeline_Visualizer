#include "CPU.h"
#include "ExecutionRecorder.h"

void overloadConfig(char* argv[])
{
  std::string arg;
  byte value;
  for (int i = 0; argv[i] != nullptr; i += 2)
  {
    arg = argv[i];
    value = std::stoi(argv[i + 1]);

    if (arg == "--clock-period-millis")
    {
      CLOCK_PERIOD_MILLIS = value;
    }
    else if (arg == "--ic-cycles-per-op")
    {
      IC_CYCLES_PER_OP = value;
    }
    else if (arg == "--ic-cycles-per-op-with-cache-hit")
    {
      IC_CYCLES_PER_OP_WITH_CACHE_HIT = value;
    }
    else if (arg == "--ls-cycles-per-op")
    {
      LS_CYCLES_PER_OP = value;
    }
    else if (arg == "--ls-cycles-per-op-with-cache-hit")
    {
      LS_CYCLES_PER_OP_WITH_CACHE_HIT = value;
    }
    else if (arg == "--de-cycles-per-op")
    {
      DE_CYCLES_PER_OP = value;
    }
    else if (arg == "--ex-cycles-per-op")
    {
      EX_CYCLES_PER_OP = value;
    }
    else if (arg == "--garbage-memory")
    {
      GARBAGE_MEMORY = value;
    }
  }
}

int main(int argc, char** argv)
{
  overloadConfig(argv+3);

  std::shared_ptr<Memory> mem;
  try
  {
    mem = std::make_shared<Memory>(argv[1], GARBAGE_MEMORY);
  } catch (const char* ex)
  {
    std::cerr << "Failure: Invalid input file\n";
    exit(EXIT_FAILURE);
  }
  
  std::shared_ptr<ExecutionRecorder> recorder = std::make_shared<ExecutionRecorder>(mem);
  
  CPU cpu(mem, recorder);
  cpu.runSimulation();

  recorder->dumpSimulationToJSONs(argv[2]);
  
  return 0;
}