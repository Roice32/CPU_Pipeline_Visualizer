#include "ExecEndSim.h"

ExecEndSim::ExecEndSim(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
        IExecutionStrategy(commPipeWithLS, refToEX, registers), clockSyncVars(clockSyncVars) {};

void ExecEndSim::executeInstruction(Instruction instr)
{
    logComplete(refToEX->getCurrTime(),LoggablePackage { EXLogPackage(instr) });
    clockSyncVars->running = false;
}