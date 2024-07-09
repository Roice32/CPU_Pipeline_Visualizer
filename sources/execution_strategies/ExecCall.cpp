#include "ExecCall.h"

ExecCall::ExecCall(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers, ExecPush* helper):
    IExecutionStrategy(lsModule, icModule, registers)
{
    pushHelper = helper;
};

void ExecCall::executeInstruction(Instruction instr)
{
    printf("\tCalling from IP: %hu\n", regs->IP);
    word methodAddress = getFinalArgValue(instr.src1, instr.param1);
    Instruction pushInstr(PUSH, IMM);
    pushInstr.param1 = regs->IP;
    pushHelper->executeInstruction(pushInstr);
    pushInstr.param1 = regs->flags;
    printf("\tIP on stack: %hu\n", requestDataAt(regs->stackPointer + regs->stackBase));
    pushHelper->executeInstruction(pushInstr);
    for (byte reg = 0; reg < 8; ++reg)
        pushHelper->executeInstruction(Instruction(PUSH, R0 + reg));
    regs->IP = methodAddress;
}