#include "ExecCall.h"

ExecCall::ExecCall(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers, ExecPush* helper):
    IExecutionStrategy(lsModule, icModule, registers)
{
    pushHelper = helper;
};

void ExecCall::executeInstruction(Instruction instr)
{
    word methodAddress = getFinalArgValue(instr.src1, instr.param1);
    log(instr, methodAddress);
    Instruction pushInstr(PUSH, IMM);
    pushInstr.param1 = regs->IP;
    pushHelper->executeInstructionNoLog(pushInstr);
    pushInstr.param1 = regs->flags;
    pushHelper->executeInstructionNoLog(pushInstr);
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        pushHelper->executeInstructionNoLog(Instruction(PUSH, R0 + reg));
    regs->IP = methodAddress;
}

ExecCall::~ExecCall() {};