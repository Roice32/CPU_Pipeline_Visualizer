#include "ExecEndSim.h"

ExecEndSim::ExecEndSim(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, std::vector<word>>> commPipeWithLS,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
    IExecutionStrategy(commPipeWithLS, registers), clockSyncVars(clockSyncVars) {};

void ExecEndSim::executeInstruction(Instruction instr)
{
    log(instr);
    clockSyncVars->running = false;
}