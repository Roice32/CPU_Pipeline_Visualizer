#include "ExecEndSim.h"

ExecEndSim::ExecEndSim(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
        IExecutionStrategy(commPipeWithLS, refToEX, registers), clockSyncVars(clockSyncVars) {};

void ExecEndSim::executeInstruction(Instruction instr)
{
    logComplete(refToEX->getCurrTime(), LoggablePackage(instr));
    printf("\t!EX ends simulation at T=%lu!\n", clockSyncVars->cycleCount);
    clockSyncVars->running = false;
}