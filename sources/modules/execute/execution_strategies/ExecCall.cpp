#include "ExecCall.h"

ExecCall::ExecCall(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
        IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, refToEX, registers), fromDEtoMe(commPipeWithDE) {};

void ExecCall::executeInstruction(Instruction instr)
{
    // TO DO: Check that stack has enough space for this call
    word methodAddress = getFinalArgValue(instr.src1, instr.param1);
    std::vector<word> savedState;
    for (byte reg = REGISTER_COUNT - 1; reg < REGISTER_COUNT; --reg)
        savedState.push_back(*regs->registers[reg]);
    savedState.push_back(*regs->flags);
    savedState.push_back(*regs->IP + 2 * WORD_BYTES);
    *regs->stackPointer -= (REGISTER_COUNT + 2) * WORD_BYTES;
    storeDataAt(*regs->stackBase + *regs->stackPointer, REGISTER_COUNT + 2, savedState);
    *regs->IP = methodAddress;
    fromDEtoMe->sendB(methodAddress);
    logComplete(refToEX->getCurrTime(), LoggablePackage(instr, methodAddress));
}

void ExecCall::log(LoggablePackage toLog)
{
    printPlainInstruction(toLog.instr);
    printf("\nSaved state:\n");
    printf("\tIP = %hu\n\t", *regs->IP + 2 * WORD_BYTES);
    printFlagsChange(~*regs->flags, *regs->flags, false);
    printf("\n\tRegisters:");
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        printf(" %s=%hu", typeNames.at(TypeCode (R0 + reg)), *regs->registers[reg]);
    printf("\n");
}