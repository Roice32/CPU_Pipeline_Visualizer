#include "ExecEndSim.h"

ExecEndSim::ExecEndSim(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecEndSim::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    clock_time lastTick = refToEX->waitTillLastTick();
    refToEX->endSimulation();
    logComplete(lastTick, log(LoggablePackage(instr)));
    std::string endMessage = "\t!EX ends simulation at T=" + std::to_string(refToEX->getCurrTime()) + "!\n";
    logAdditional(endMessage);
}