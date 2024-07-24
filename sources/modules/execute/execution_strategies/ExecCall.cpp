#include "ExecCall.h"

ExecCall::ExecCall(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecCall::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    assert((*regs->stackPointer >= (REGISTER_COUNT + 2) * WORD_BYTES) && "Insufficient stack space for method call");

    SynchronizedDataPackage<word> methodAddressPckg = getFinalArgValue(instr.src1, instr.param1);
    if (methodAddressPckg.exceptionTriggered)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
            methodAddressPckg.excpData,
            MISALIGNED_ACCESS_HANDL));
        return;
    }
    
    std::vector<word> savedState;
    for (byte reg = REGISTER_COUNT - 1; reg < REGISTER_COUNT; --reg)
        savedState.push_back(*regs->registers[reg]);
    savedState.push_back(*regs->flags);
    savedState.push_back(*regs->IP + 2 * WORD_BYTES);
    *regs->stackPointer -= (REGISTER_COUNT + 2) * WORD_BYTES;
    SynchronizedDataPackage<std::vector<word>> storeResultPckg = storeDataAt(*regs->stackBase + *regs->stackPointer, REGISTER_COUNT + 2, savedState);
    clock_time lastTick = refToEX->waitTillLastTick();
    if (storeResultPckg.exceptionTriggered)
    {
        handleException(SynchronizedDataPackage<Instruction> (*regs->IP,
            storeResultPckg.excpData,
            MISALIGNED_ACCESS_HANDL));
        return;
    }
    logComplete(lastTick, log(LoggablePackage(instr, methodAddressPckg.data)));
    *regs->IP = methodAddressPckg.data;
    fromDEtoMe->sendB(methodAddressPckg.data);
}

std::string ExecCall::log(LoggablePackage toLog)
{
    std::string result = plainInstructionToString(toLog.instr) + "\nSaved state:\n";
    result += "\tIP = #" + convDecToHex(*regs->IP + 2 * WORD_BYTES) + "\n\t";
    result += printFlagsChange(~*regs->flags, *regs->flags, false);
    result += "\n\tRegisters:";
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
    {
        result += " ";
        result += typeNames.at(TypeCode (R0 + reg));
        result += "=" + std::to_string(*regs->registers[reg]);
    }
    result += "\n";
    return result;
}