#include "ExecExcpExit.h"

ExecExcpExit::ExecExcpExit(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecExcpExit::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    SynchronizedDataPackage<std::vector<word>> restoredStatePckg = requestDataAt(SAVE_STATE_ADDR, REGISTER_COUNT + 4);
    
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        *regs->registers[reg] = restoredStatePckg.data[4 + reg];
    *regs->flags = restoredStatePckg.data[2];
    *regs->stackPointer = restoredStatePckg.data[1];
    *regs->IP = restoredStatePckg.data[0];
    *regs->flags &= ~EXCEPTION;

    fromDEtoMe->sendB(restoredStatePckg.data[0]);
    clock_time lastTick = refToEX->waitTillLastTick();
    logComplete(lastTick, log(LoggablePackage(instr)));   
}

std::string ExecExcpExit::log(LoggablePackage toLog)
{
    std::string result = "Finished executing: " + plainInstructionToString(toLog.instr) + "\nReturned from exception handler to state:\n";
    result += "\tIP = #" + convDecToHex(*regs->IP);
    result += "\n\tSP = " + std::to_string(*regs->stackPointer);
    result += "\n\t" + printFlagsChange(~*regs->flags, *regs->flags, false);
    result += "\n\tRegisters:";
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
    {
        result += " ";
        result += typeNames.at(TypeCode(R0 + reg));
        result += "=" + std::to_string(*regs->registers[reg]);
    }
    result += "\n\tException flag cleared\n";
    return result;
}