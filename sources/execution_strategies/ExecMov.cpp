#include "ExecMov.h"

ExecMov::ExecMov(InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithLS, CPURegisters* registers):
    IExecutionStrategy(commPipeWithLS, registers) {};

void ExecMov::executeInstruction(Instruction instr)
{
    word movedValue = getFinalArgValue(instr.src2, instr.param2);
    log(instr, 0, movedValue); 
    storeResultAtDest(movedValue, instr.src1, instr.param1);
}

void ExecMov::log(Instruction instr, word actualParam1, word actualParam2, bool newLine)
{
    printf(">");
    printPlainInstruction(instr);
    printf(" (");
    printPlainArg(instr.src1, instr.param1, false);
    printf(" = %hu)\n", actualParam2);
}

ExecMov::~ExecMov() {};