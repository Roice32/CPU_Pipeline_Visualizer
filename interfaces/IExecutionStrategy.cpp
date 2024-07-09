#pragma once

#include "Execute.h"
#include "Instruction.h"
#include <cstdio>

class IExecutionStrategy
{
protected:
    Execute* EXModule;

    word getFinalArgValue(byte src, word param = 0)
    {
        CPURegisters* regs = EXModule->registers;
        if (src == NULL_VAL)
            return 0;
        if (src == IMM)
            return param;
        if (src == ADDR)
            return EXModule->requestDataAt(param);
        if (src == SP_REG)
            return regs->stackPointer;
        if (src == ST_BASE)
            return regs->stackBase;
        if (src == ST_SIZE)
            return regs->stackSize;
        if (src >= R0 && src <= R7)
            return regs->registers[src - R0];
        if (src >= ADDR_R0 && src <= ADDR_R7)
            return EXModule->requestDataAt(regs->registers[src - R0]);
        throw "Wrong or unimplemented argument type";
    }

    void storeResultAtDest(word result, byte destType, byte destLocation = 0)
    {
        CPURegisters* regs = EXModule->registers;
        if (destType == ADDR)
            EXModule->storeDataAt(destLocation, result);
        else if (destType == SP_REG)
            regs->stackPointer = result;
        else if (destType == ST_BASE)
            regs->stackBase = result;
        else if (destType == ST_SIZE)
            regs->stackSize = result;
        else if (destType >= R0 && destType <= R7)
            regs->registers[destType - R0] = result;
        else if (destType >= ADDR_R0 && destType <= ADDR_R7)
            EXModule->storeDataAt(regs->registers[destType - ADDR_R0], result);
        throw "Wrong or unimplemented argument type";
    }

public:
    // A constructor needs to go here
    virtual void executeInstruction(Instruction instr) = 0;
    void log(Instruction instr)
    {
        // Later
    }
};