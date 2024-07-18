#include "ExecEndSim.h"

ExecEndSim::ExecEndSim(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
    IExecutionStrategy(commPipeWithLS, registers), clockSyncVars(clockSyncVars) {};

void ExecEndSim::executeInstruction(Instruction instr)
{
    log(LoggablePackage { EXLogPackage(instr) });
    clockSyncVars->running = false;
}