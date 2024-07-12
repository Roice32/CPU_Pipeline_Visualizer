#include "ExecRet.h"

ExecRet::ExecRet(InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithLS, CPURegisters* registers, ExecPop* helper):
    IExecutionStrategy(commPipeWithLS, registers)
{
    popHelper = helper;
};

void ExecRet::executeInstruction(Instruction instr)
{
    for (byte reg = 7; reg < REGISTER_COUNT; --reg)
        popHelper->executeInstructionNoLog(Instruction(POP, R0 + reg));
    
    word currentSP = regs->stackBase + regs->stackPointer;
    regs->flags = requestDataAt(currentSP);
    popHelper->executeInstructionNoLog(Instruction(POP, NULL_VAL));
    currentSP += 2;
    
    regs->IP = requestDataAt(currentSP);
    popHelper->executeInstructionNoLog(Instruction(POP, NULL_VAL));
    log(instr);
}

void ExecRet::log(Instruction instr, word actualParam1, word actualParam2, bool newLine)
{
    printf(">");
    printPlainInstruction(instr);
    printf("\nReturned to state:\n");
    printf("\tIP = %hu\n\t", regs->IP);
    printFlagsChange(~regs->flags, regs->flags, false);
    printf("\n\tRegisters:");
    for (byte reg = 0; reg < 8; ++reg)
        printf(" %s=%hu", typeNames.at(TypeCode (R0 + reg)), regs->registers[R0 + reg]);
    printf("\n");
}

ExecRet::~ExecRet() {};