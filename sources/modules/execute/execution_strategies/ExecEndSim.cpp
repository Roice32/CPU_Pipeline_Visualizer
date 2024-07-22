#include "ExecEndSim.h"

ExecEndSim::ExecEndSim(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
        IExecutionStrategy(commPipeWithLS, refToEX, registers), clockSyncVars(clockSyncVars) {};

void ExecEndSim::executeInstruction(Instruction instr)
{
    clock_time lastTick = refToEX->waitTillLastTick();
    clockSyncVars->running = false;
    logComplete(lastTick, log(LoggablePackage(instr)));
    std::string endMessage = "\t!EX ends simulation at T=" + std::to_string(clockSyncVars->cycleCount) + "!\n";
    logAdditional(endMessage);
}