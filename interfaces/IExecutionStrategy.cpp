#pragma once

#include "CPURegisters.h"
#include "EXLogger.h"
#include "IMemoryAccesser.cpp"
#include "IClockBoundModule.cpp"
#include <cassert>

class IExecutionStrategy: public IMemoryAccesser, public EXLogger
{
protected:
    IClockBoundModule* refToEX;
    std::shared_ptr<CPURegisters> regs;

    IExecutionStrategy(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        IClockBoundModule* refToEX,
        std::shared_ptr<CPURegisters> registers):
            IMemoryAccesser(commPipeWithLS), EXLogger(), refToEX(refToEX), regs(registers) {};

    word getFinalArgValue(byte src, word param = 0)
    {
        switch(src)
        {
            case NULL_VAL:
                return 0;
            case IMM:
                return param;
            case ADDR:
                return requestDataAt(param, 1)[0];
            case SP_REG:
                return *regs->stackPointer;
            case ST_BASE:
                return *regs->stackBase;
            case ST_SIZE:
                return *regs->stackSize;
            case R0 ... R7:
                return *regs->registers[src - R0];
            case ADDR_R0 ... ADDR_R7:
                return requestDataAt(*regs->registers[src - ADDR_R0], 1)[0];
            default:
                assert(0 && "Wrong or unimplemented parameter type");
        }
    }

    void storeResultAtDest(word result, byte destType, word destLocation = 0)
    {
        switch (destType)
        {
            case ADDR:
                storeDataAt(destLocation, 1, std::vector<word> { result });
            break;
            case SP_REG:
                *regs->stackPointer = result;
            break;
            case ST_BASE:
                *regs->stackBase = result;
            break;
            case ST_SIZE:
                *regs->stackSize = result;
            break;
            case R0 ... R7:
                *regs->registers[destType - R0] = result;
            break;
            case ADDR_R0 ... ADDR_R7:
                storeDataAt(*regs->registers[destType - ADDR_R0], 1, std::vector<word> { result });
            break;
            default:
                assert(0 && "Wrong or unimplemented parameter type");
        }
    }

    std::vector<word> requestDataAt(address addr, byte howManyWords)
    {
        MemoryAccessRequest newReq(addr, howManyWords);
        fromEXtoLS->sendA(newReq);
        refToEX->enterIdlingState();
        while (!fromEXtoLS->pendingB())
            refToEX->awaitClockSignal();
        refToEX->returnFromIdlingState();
        return fromEXtoLS->getB().data;
    }

    void storeDataAt(address addr, byte howManyWords, std::vector<word> data)
    {
        MemoryAccessRequest newReq(addr, howManyWords, true, data);
        fromEXtoLS->sendA(newReq);
        refToEX->enterIdlingState();
        while (!fromEXtoLS->pendingB())
            refToEX->awaitClockSignal();
        refToEX->returnFromIdlingState();
        fromEXtoLS->getB();
        return;
    }

    void moveIP(Instruction instr)
    {
        *regs->IP += WORD_BYTES;
        if (instr.src1 == IMM || instr.src1 == ADDR)
            *regs->IP += WORD_BYTES;
        if (instr.src2 == IMM || instr.src2 == ADDR)
            *regs->IP += WORD_BYTES;
    }

public:
    virtual void executeInstruction(Instruction instr) = 0;
};