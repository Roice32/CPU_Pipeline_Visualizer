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
                assert(result <= *regs->stackSize && "Attempt to move stack pointer over the stack's size");
                assert(result % 2 == 0 && "Attempt to misalign stack pointer");
                *regs->stackPointer = result;
            break;
            case ST_BASE:
                assert(result % 2 == 0 && "Attempt to misalign stack base");
                *regs->stackBase = result;
            break;
            case ST_SIZE:
                assert(result >= *regs->stackPointer && "Attempt to shrink stack below current stack pointer");
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
        SynchronizedDataPackage<MemoryAccessRequest> syncReq(newReq, addr);
        clock_time currTick = refToEX->getCurrTime();
        syncReq.sentAt = currTick;
        fromEXtoLS->sendA(syncReq);
        refToEX->enterIdlingState();
        while (!fromEXtoLS->pendingB()) // TO DO: Check running here
            refToEX->returnFromIdlingState();
        SynchronizedDataPackage<std::vector<word>> receivedPckg = fromEXtoLS->getB();
        refToEX->awaitNextTickToHandle(receivedPckg);
        return receivedPckg.data;
    }

    void storeDataAt(address addr, byte howManyWords, std::vector<word> data)
    {
        MemoryAccessRequest newReq(addr, howManyWords, true, data);
        SynchronizedDataPackage<MemoryAccessRequest> syncReq(newReq, addr);
        clock_time currTick = refToEX->getCurrTime();
        syncReq.sentAt = currTick;
        fromEXtoLS->sendA(syncReq);
        refToEX->enterIdlingState();
        while (!fromEXtoLS->pendingB())
            refToEX->returnFromIdlingState();
        SynchronizedDataPackage<std::vector<word>> receivedPckg = fromEXtoLS->getB();
        refToEX->awaitNextTickToHandle(receivedPckg);
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