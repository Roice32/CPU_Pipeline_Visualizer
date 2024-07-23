#include "ExecRet.h"

ExecRet::ExecRet(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, refToEX, registers), fromDEtoMe(commPipeWithDE) {};

void ExecRet::executeInstruction(Instruction instr)
{
    assert((*regs->stackSize - *regs->stackPointer >= (REGISTER_COUNT + 2) * WORD_BYTES) && "Stack too empty to consider return from method");
    std::vector<word> restoredState = requestDataAt(*regs->stackBase + *regs->stackPointer, REGISTER_COUNT + 2);
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        *regs->registers[REGISTER_COUNT - 1 - reg] = restoredState[reg];
    *regs->flags = restoredState[REGISTER_COUNT];
    *regs->IP = restoredState[REGISTER_COUNT + 1];
    *regs->stackPointer += (REGISTER_COUNT + 2) * WORD_BYTES;
    fromDEtoMe->sendB(restoredState[REGISTER_COUNT + 1]);
    clock_time lastTick = refToEX->waitTillLastTick();
    logComplete(lastTick, log(LoggablePackage(instr)));
}

std::string ExecRet::log(LoggablePackage toLog)
{
    std::string result = plainInstructionToString(toLog.instr) + "\nReturned to state:\n";
    result += "\tIP = #" + convDecToHex(*regs->IP);
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