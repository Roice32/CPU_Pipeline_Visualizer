#pragma once

#include "Instruction.h"
#include "InstructionCache.h"
#include "CPURegisters.h"
#include "IMemoryAccesser.cpp"
#include <cstdio>

class IExecutionStrategy: public IMemoryAccesser
{
protected:
    InstructionCache* ICModule;
    CPURegisters* regs;

    word getFinalArgValue(byte src, word param = 0)
    {
        if (src == NULL_VAL)
            return 0;
        if (src == IMM)
            return param;
        if (src == ADDR)
            return requestDataAt(param);
        if (src == SP_REG)
            return regs->stackPointer;
        if (src == ST_BASE)
            return regs->stackBase;
        if (src == ST_SIZE)
            return regs->stackSize;
        if (src >= R0 && src <= R7)
            return regs->registers[src - R0];
        if (src >= ADDR_R0 && src <= ADDR_R7)
            return requestDataAt(regs->registers[src - ADDR_R0]);
        throw "Wrong or unimplemented argument type";
    }

    void storeResultAtDest(word result, byte destType, word destLocation = 0)
    {
        if (destType == ADDR)
            storeDataAt(destLocation, result);
        else if (destType == SP_REG)
            regs->stackPointer = result;
        else if (destType == ST_BASE)
            regs->stackBase = result;
        else if (destType == ST_SIZE)
            regs->stackSize = result;
        else if (destType >= R0 && destType <= R7)
            regs->registers[destType - R0] = result;
        else if (destType >= ADDR_R0 && destType <= ADDR_R7)
            storeDataAt(regs->registers[destType - ADDR_R0], result);
        else throw "Wrong or unimplemented argument type";
    }

public:
    IExecutionStrategy(LoadStore* lsModule, InstructionCache* icModule, CPURegisters* registers): IMemoryAccesser(lsModule)
    {
        ICModule = icModule;
        regs = registers;
    }
    word requestDataAt(word addr)
    {
        word result = 0;
        result = LSModule->loadFrom(addr);
        result <<= 8;
        result |= LSModule->loadFrom(addr + 1);
        return result;
    }
    void storeDataAt(word addr, word data)
    {
        LSModule->storeAt(addr, data >> 8);
        LSModule->storeAt(addr + 1, (data << 8) >> 8);
    }
    virtual void executeInstruction(Instruction instr) = 0;
    void log(Instruction instr)
    {
        // Later
    }
};