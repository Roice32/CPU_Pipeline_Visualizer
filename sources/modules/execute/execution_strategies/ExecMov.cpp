#include "ExecMov.h"

ExecMov::ExecMov(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, refToEX, registers) {};

void ExecMov::executeInstruction(Instruction instr)
{
    word movedValue = getFinalArgValue(instr.src2, instr.param2);
    logComplete(refToEX->getCurrTime(), LoggablePackage(instr, 0, movedValue)); 
    storeResultAtDest(movedValue, instr.src1, instr.param1);
    moveIP(instr);
}

void ExecMov::log(LoggablePackage toLog)
{
    printPlainInstruction(toLog.instr);
    printf(" (");
    printPlainArg(toLog.instr.src1, toLog.instr.param1, false);
    printf(" = %hu)\n", toLog.actualParam2);
}