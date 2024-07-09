#pragma once

#include "ExecJumpOp.h"
#include "ExecSimpleMathOp.h"
#include "ExecComplexMathOp.h"
#include "ExecMov.h"
#include "ExecCmp.h"

class ExecutionStrategyPool
{
public:
    ExecSimpleMathOp* addOrSub;
    ExecComplexMathOp* mulOrDiv;
    ExecMov* mov;
    ExecCmp* cmp;
    ExecJumpOp* jmpOp;

    ExecutionStrategyPool(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    ~ExecutionStrategyPool();
};