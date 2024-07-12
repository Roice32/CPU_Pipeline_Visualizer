#include "ExecPush.h"

ExecPush::ExecPush(InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithLS, CPURegisters* registers):
    IExecutionStrategy(commPipeWithLS, registers) {};

void ExecPush::executeInstruction(Instruction instr)
{
    word actualParam = getFinalArgValue(instr.src1, instr.param1);
    log(instr, actualParam);
    executeInstructionNoLog(instr);
}

void ExecPush::executeInstructionNoLog(Instruction instr)
{
    if (regs->stackPointer < 2)
        throw "Upper limit of the stack exceeded";
    word actualParam = getFinalArgValue(instr.src1, instr.param1);
    regs->stackPointer -= 2;
    word newSP = regs->stackBase + regs->stackPointer;
    storeDataAt(newSP, actualParam);
}

ExecPush::~ExecPush() {};