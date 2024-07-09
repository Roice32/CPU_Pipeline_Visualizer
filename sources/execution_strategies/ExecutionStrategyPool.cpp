#include "ExecutionStrategyPool.h"

ExecutionStrategyPool::ExecutionStrategyPool()
{
    add = new ExecuteADD();
    // Add more as I progress
}

ExecutionStrategyPool::~ExecutionStrategyPool()
{
    delete add;
}