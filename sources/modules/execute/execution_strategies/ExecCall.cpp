#include "ExecCall.h"

ExecCall::ExecCall(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
        IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, refToEX, registers), fromDEtoMe(commPipeWithDE) {};

void ExecCall::executeInstruction(Instruction instr)
{
    assert((*regs->stackPointer >= (REGISTER_COUNT + 2) * WORD_BYTES) && "Insufficient stack space for method call");
    word methodAddress = getFinalArgValue(instr.src1, instr.param1);
    std::vector<word> savedState;
    for (byte reg = REGISTER_COUNT - 1; reg < REGISTER_COUNT; --reg)
        savedState.push_back(*regs->registers[reg]);
    savedState.push_back(*regs->flags);
    savedState.push_back(*regs->IP + 2 * WORD_BYTES);
    *regs->stackPointer -= (REGISTER_COUNT + 2) * WORD_BYTES;
    storeDataAt(*regs->stackBase + *regs->stackPointer, REGISTER_COUNT + 2, savedState);
    clock_time lastTick = refToEX->waitTillLastTick();
    logComplete(lastTick, log(LoggablePackage(instr, methodAddress)));
    *regs->IP = methodAddress;
    fromDEtoMe->sendB(methodAddress);
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