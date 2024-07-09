#include "ExecutionStrategyPool.h"

ExecutionStrategyPool::ExecutionStrategyPool(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers)
{
    addOrSub = new ExecuteSimpleMathOp(lsModule, icModule, registers);
    // Add more as I progress
}

ExecutionStrategyPool::~ExecutionStrategyPool()
{
    delete addOrSub;
}