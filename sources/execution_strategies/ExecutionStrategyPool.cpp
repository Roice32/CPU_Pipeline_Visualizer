#include "ExecutionStrategyPool.h"

ExecutionStrategyPool::ExecutionStrategyPool(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers)
{
    addOrSub = new ExecSimpleMathOp(lsModule, icModule, registers);
    mulOrDiv = new ExecComplexMathOp(lsModule, icModule, registers);
    mov = new ExecMov(lsModule, icModule, registers);
    cmp = new ExecCmp(lsModule, icModule, registers);
    jmpOp = new ExecJumpOp(lsModule, icModule, registers);
    push = new ExecPush(lsModule, icModule, registers);
    pop = new ExecPop(lsModule, icModule, registers);
    call = new ExecCall(lsModule, icModule, registers, push);
    ret = new ExecRet(lsModule, icModule, registers, pop);
    // Add more as I progress
}

ExecutionStrategyPool::~ExecutionStrategyPool()
{
    delete addOrSub;
    delete mulOrDiv;
    delete mov;
    delete cmp;
    delete jmpOp;
    delete push;
    delete pop;
    delete call;
    delete pop;
    delete ret;
    // Also remember to delete them here
}