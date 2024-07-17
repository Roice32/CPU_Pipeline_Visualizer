#include "ExecCall.h"

ExecCall::ExecCall(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, std::vector<word>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers):
    IExecutionStrategy(commPipeWithLS, registers) {};

void ExecCall::executeInstruction(Instruction instr)
{
    word methodAddress = getFinalArgValue(instr.src1, instr.param1);
    log(instr, methodAddress);
    std::vector<word> savedState;
    for (byte reg = 7; reg < REGISTER_COUNT; --reg)
        savedState.push_back(*regs->registers[reg]);
    savedState.push_back(*regs->flags);
    savedState.push_back(*regs->IP + 4);
    *regs->stackPointer -= 10 * WORD_BYTES;
    storeDataAt(*regs->stackBase + *regs->stackPointer, 10, savedState);
    *regs->IP = methodAddress;
}

void ExecCall::log(Instruction instr, word actualparam1, word actualParam2, bool newLine)
{
    printf(">");
    printPlainInstruction(instr);
    printf("\nSaved state:\n");
    printf("\tIP = %hu\n\t", *regs->IP + 4);
    printFlagsChange(~*regs->flags, *regs->flags, false);
    printf("\n\tRegisters:");
    for (byte reg = 0; reg < 8; ++reg)
        printf(" %s=%hu", typeNames.at(TypeCode (R0 + reg)), *regs->registers[reg]);
    printf("\n");
}