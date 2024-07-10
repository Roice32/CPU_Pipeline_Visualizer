#include "ExecEndSim.h"

ExecEndSim::ExecEndSim(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers):
    IExecutionStrategy(lsModule, icModule, registers) {};

void ExecEndSim::executeInstruction(Instruction instr)
{
    log(instr);
    // Temp termination condition until multithreading is implemented
    regs->IP = 0xffff;
}

ExecEndSim::~ExecEndSim() {};