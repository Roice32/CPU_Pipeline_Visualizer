#include "ExecRet.h"

ExecRet::ExecRet(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers, ExecPop* helper):
    IExecutionStrategy(lsModule, icModule, registers)
{
    popHelper = helper;
};

void ExecRet::executeInstruction(Instruction instr)
{
    log(instr);
    for (byte reg = 7; reg < 8; --reg)
        popHelper->executeInstructionNoLog(Instruction(POP, R0 + reg));
    
    word currentSP = regs->stackBase + regs->stackPointer;
    regs->flags = requestDataAt(currentSP);
    popHelper->executeInstructionNoLog(Instruction(POP, NULL_VAL));
    currentSP += 2;
    
    regs->IP = requestDataAt(currentSP);
    popHelper->executeInstructionNoLog(Instruction(POP, NULL_VAL));
}