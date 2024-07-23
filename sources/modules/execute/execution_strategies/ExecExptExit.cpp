#include "ExecExcpExit.h"

ExecExcpExit::ExecExcpExit(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecExcpExit::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    std::vector<word> restoredState = requestDataAt(SAVE_STATE_ADDR, REGISTER_COUNT + 4);
    
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        *regs->registers[reg] = restoredState[4 + reg];
    *regs->flags = restoredState[2];
    *regs->stackPointer = restoredState[1];
    *regs->IP = restoredState[0];
    *regs->flags &= ~EXCEPTION;

    fromDEtoMe->sendB(restoredState[0]);
    clock_time lastTick = refToEX->waitTillLastTick();
    logComplete(lastTick, log(LoggablePackage(instr)));   
}

std::string ExecExcpExit::log(LoggablePackage toLog)
{
    std::string result = plainInstructionToString(toLog.instr) + "\nReturned from exception handler to state:\n";
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
    result += "\n";
    return result;
}