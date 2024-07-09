#pragma once

#include "ExecuteADD.h"

class ExecutionStrategyPool
{
public:
    ExecuteADD* add;

    ExecutionStrategyPool();
    ~ExecutionStrategyPool();
};