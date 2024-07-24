#include "ExecRet.h"

ExecRet::ExecRet(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecRet::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;

    if(*regs->stackSize < *regs->stackPointer || *regs->stackSize - *regs->stackPointer < (REGISTER_COUNT + 2) * WORD_BYTES)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
            POP_OVERFLOW,
            STACK_OVERFLOW_HANDL));
        return;
    }

    SynchronizedDataPackage<std::vector<word>> restoredStatePckg = requestDataAt(*regs->stackBase + *regs->stackPointer, REGISTER_COUNT + 2);

    if (restoredStatePckg.exceptionTriggered)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
            restoredStatePckg.excpData,
            MISALIGNED_ACCESS_HANDL));
        return;
    }

    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        *regs->registers[REGISTER_COUNT - 1 - reg] = restoredStatePckg.data[reg];
    *regs->flags = restoredStatePckg.data[REGISTER_COUNT];
    *regs->IP = restoredStatePckg.data[REGISTER_COUNT + 1];
    *regs->stackPointer += (REGISTER_COUNT + 2) * WORD_BYTES;
    fromDEtoMe->sendB(restoredStatePckg.data[REGISTER_COUNT + 1]);
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