#pragma once

#include "ExecSimpleMathOp.h"
#include "ExecComplexMathOp.h"
#include "ExecMov.h"

class ExecutionStrategyPool
{
public:
    ExecSimpleMathOp* addOrSub;
    ExecComplexMathOp* mulOrDiv;
    ExecMov* mov;

    ExecutionStrategyPool(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    ~ExecutionStrategyPool();
};