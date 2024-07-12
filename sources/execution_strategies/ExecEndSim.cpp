#include "ExecEndSim.h"

ExecEndSim::ExecEndSim(InterThreadCommPipe<MemoryAccessRequest, word>* commPipeWithLS, CPURegisters* registers):
    IExecutionStrategy(commPipeWithLS, registers) {};

void ExecEndSim::executeInstruction(Instruction instr)
{
    log(instr);
    // TO DO: Proper termination condition via signals when multithreading is implemented
    regs->IP = 0xffff;
    regs->flags &= ~RUNNING;
}

ExecEndSim::~ExecEndSim() {};