#pragma once

#include "CPURegisters.h"
#include "ExecutionLogger.h"
#include "IMemoryAccesser.cpp"

class IExecutionStrategy: public IMemoryAccesser, public ExecutionLogger
{
protected:
    std::shared_ptr<CPURegisters> regs;

    IExecutionStrategy(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS, std::shared_ptr<CPURegisters> registers):
        IMemoryAccesser(commPipeWithLS), ExecutionLogger(), regs(registers) {};

    word getFinalArgValue(byte src, word param = 0)
    {
        if (src == NULL_VAL)
            return 0;
        if (src == IMM)
            return param;
        if (src == ADDR)
            return requestDataAt(param);
        if (src == SP_REG)
            return *regs->stackPointer;
        if (src == ST_BASE)
            return *regs->stackBase;
        if (src == ST_SIZE)
            return *regs->stackSize;
        if (src >= R0 && src <= R7)
            return *regs->registers[src - R0];
        if (src >= ADDR_R0 && src <= ADDR_R7)
            return requestDataAt(*regs->registers[src - ADDR_R0]);
        throw "Wrong or unimplemented argument type";
    }

    void storeResultAtDest(word result, byte destType, word destLocation = 0)
    {
        if (destType == ADDR)
            storeDataAt(destLocation, result);
        else if (destType == SP_REG)
            *regs->stackPointer = result;
        else if (destType == ST_BASE)
            *regs->stackBase = result;
        else if (destType == ST_SIZE)
            *regs->stackSize = result;
        else if (destType >= R0 && destType <= R7)
            *regs->registers[destType - R0] = result;
        else if (destType >= ADDR_R0 && destType <= ADDR_R7)
            storeDataAt(*regs->registers[destType - ADDR_R0], result);
        else throw "Wrong or unimplemented argument type";
    }

    word requestDataAt(word addr)
    {
        MemoryAccessRequest newReq(addr);
        requestsToLS->sendRequest(newReq);
        // TO DO somehow else
        while (!requestsToLS->pendingResponse()) ;
        return requestsToLS->getResponse();
    }

    void storeDataAt(word addr, word data)
    {
        MemoryAccessRequest newReq(addr, true, data);
        requestsToLS->sendRequest(newReq);
        // TO DO
        while (!requestsToLS->pendingResponse()) ;
        requestsToLS->getResponse();
        return;
    }

public:
    virtual void executeInstruction(Instruction instr) = 0;
};