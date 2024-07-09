#pragma once

#include "ExecuteSimpleMathOp.h"

class ExecutionStrategyPool
{
public:
    ExecuteSimpleMathOp* addOrSub;

    ExecutionStrategyPool(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    ~ExecutionStrategyPool();
};