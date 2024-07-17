#include "ExecRet.h"

ExecRet::ExecRet(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, std::vector<word>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers):
    IExecutionStrategy(commPipeWithLS, registers) {};

void ExecRet::executeInstruction(Instruction instr)
{
    std::vector<word> restoredState = requestDataAt(*regs->stackBase + *regs->stackPointer, 10);
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        *regs->registers[7 - reg] = restoredState[reg];
    *regs->flags = restoredState[8];
    *regs->IP = restoredState[9];
    *regs->stackPointer += 10 * WORD_BYTES;

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
    for (byte reg = 0; reg < 8; ++reg)
        printf(" %s=%hu", typeNames.at(TypeCode (R0 + reg)), *regs->registers[reg]);
    printf("\n");
}