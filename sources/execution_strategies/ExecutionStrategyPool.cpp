#include "ExecutionStrategyPool.h"

ExecutionStrategyPool::ExecutionStrategyPool(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers)
{
    addOrSub = new ExecSimpleMathOp(lsModule, icModule, registers);
    mulOrDiv = new ExecComplexMathOp(lsModule, icModule, registers);
    mov = new ExecMov(lsModule, icModule, registers);
    cmp = new ExecCmp(lsModule, icModule, registers);
    jmpOp = new ExecJumpOp(lsModule, icModule, registers);
    // Add more as I progress
}

ExecutionStrategyPool::~ExecutionStrategyPool()
{
    delete addOrSub;
    delete mulOrDiv;
    delete mov;
    delete cmp;
    delete jmpOp;
    // Also remember to delete them here
}