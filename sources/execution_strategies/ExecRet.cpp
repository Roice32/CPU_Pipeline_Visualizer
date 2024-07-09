#include "ExecRet.h"

ExecRet::ExecRet(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers, ExecPop* helper):
    IExecutionStrategy(lsModule, icModule, registers)
{
    popHelper = helper;
};

void ExecRet::executeInstruction(Instruction instr)
{
    for (byte reg = 7; reg < 8; --reg)
        popHelper->executeInstruction(Instruction(POP, R0 + reg));
    
    word currentSP = regs->stackBase + regs->stackPointer;
    regs->flags = requestDataAt(currentSP);
    popHelper->executeInstruction(Instruction(POP, NULL_VAL));
    currentSP += 2;
    
    printf("\tReturning from IP: %hu\n", regs->IP);
    regs->IP = requestDataAt(currentSP);
    popHelper->executeInstruction(Instruction(POP, NULL_VAL));
    printf("\tReturned to IP: %hu\n", regs->IP);
}