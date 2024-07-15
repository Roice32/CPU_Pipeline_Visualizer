#include "ExecCall.h"

ExecCall::ExecCall(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS, std::shared_ptr<CPURegisters> registers, std::shared_ptr<ExecPush> helper):
    IExecutionStrategy(commPipeWithLS, registers), pushHelper(helper) {};

void ExecCall::executeInstruction(Instruction instr)
{
    word methodAddress = getFinalArgValue(instr.src1, instr.param1);
    log(instr, methodAddress);
    Instruction pushInstr(PUSH, IMM);
    pushInstr.param1 = *regs->IP + 4;
    pushHelper->executeInstructionNoLog(pushInstr);
    pushInstr.param1 = *regs->flags;
    pushHelper->executeInstructionNoLog(pushInstr);
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        pushHelper->executeInstructionNoLog(Instruction(PUSH, R0 + reg));
    *regs->IP = methodAddress;
}

void ExecCall::log(Instruction instr, word actualparam1, word actualParam2, bool newLine)
{
    printf(">");
    printPlainInstruction(instr);
    printf("\nSaved state:\n");
    printf("\tIP = %hu\n\t", *regs->IP);
    printFlagsChange(~*regs->flags, *regs->flags, false);
    printf("\n\tRegisters:");
    for (byte reg = 0; reg < 8; ++reg)
        printf(" %s=%hu", typeNames.at(TypeCode (R0 + reg)), *regs->registers[reg]);
    printf("\n");
}