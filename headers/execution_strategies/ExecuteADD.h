#pragma once

#include "IExecutionStrategy.cpp"
class ExecuteADD: public IExecutionStrategy
{
public:
    void executeInstruction(Instruction instr);
};