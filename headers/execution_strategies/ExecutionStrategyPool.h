#pragma once

#include "ExecSimpleMathOp.h"
#include "ExecComplexMathOp.h"

class ExecutionStrategyPool
{
public:
    ExecSimpleMathOp* addOrSub;
    ExecComplexMathOp* mulOrDiv;

    ExecutionStrategyPool(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    ~ExecutionStrategyPool();
};