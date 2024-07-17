#include "ExecRet.h"

ExecRet::ExecRet(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, std::vector<word>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers):
    IExecutionStrategy(commPipeWithLS, registers) {};

void ExecRet::executeInstruction(Instruction instr)
{
    std::vector<word> restoredState = requestDataAt(*regs->stackBase + *regs->stackPointer, 10);
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        *regs->registers[REGISTER_COUNT - 1 - reg] = restoredState[reg];
    *regs->flags = restoredState[REGISTER_COUNT];
    *regs->IP = restoredState[REGISTER_COUNT + 1];
    *regs->stackPointer += (REGISTER_COUNT + 2) * WORD_BYTES;

    log(instr);
}

void ExecRet::log(Instruction instr, word actualParam1, word actualParam2, bool newLine)
{
    printf(">");
    printPlainInstruction(instr);
    printf("\nReturned to state:\n");
    printf("\tIP = %hu\n\t", *regs->IP);
    printFlagsChange(~*regs->flags, *regs->flags, false);
    printf("\n\tRegisters:");
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        printf(" %s=%hu", typeNames.at(TypeCode (R0 + reg)), *regs->registers[reg]);
    printf("\n");
}