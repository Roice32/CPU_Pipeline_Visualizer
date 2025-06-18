#include "ExecEndSim.h"

void ExecEndSim::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
  Instruction instr = instrPackage.data;
  recorder->modifyModuleState(EX, "Executing " + instr.toString());
  clock_time lastTick = refToEX->waitTillLastTick();
  refToEX->endSimulation();
  recorder->addExtraInfo(EX, "Ended simulation");
  recorder->setSimEndReason(NORMAL);
}