#pragma once

#include "ExecCall.h"
#include "ExecJumpOp.h"
#include "ExecPop.h"
#include "ExecPush.h"
#include "ExecRet.h"
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
    ExecPush* push;
    ExecPop* pop;
    ExecCall* call;
    ExecRet* ret;

    ExecutionStrategyPool(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers);
    ~ExecutionStrategyPool();
};