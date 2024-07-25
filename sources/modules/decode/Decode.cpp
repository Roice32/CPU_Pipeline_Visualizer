#include "Decode.h"
#include <cassert>

Decode::Decode(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<fetch_window>, address>> commPipeWithIC,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithEX,
    std::shared_ptr<ClockSyncPackage> clockSyncVars,
    std::shared_ptr<register_16b> flags):
        IClockBoundModule(clockSyncVars, 2, "Decode"),
        fromICtoMe(commPipeWithIC), fromMetoEX(commPipeWithEX), flags(flags), discardUntilAddr(DUMMY_ADDRESS) {};

byte Decode::getExpectedParamCount(byte opCode)
{
    if (opCode < JMP)
        return 2;
    if (opCode == RET || opCode == END_SIM || opCode == EXCP_EXIT)
        return 0;
    return 1;
}

char Decode::providedVsExpectedArgsCountDif(byte opCode, byte src1, byte src2)
{
    byte expectedParamCount = getExpectedParamCount(opCode);
    if (expectedParamCount == 2 && src2 == NULL_VAL)
        return -1;
    if (expectedParamCount == 1 && src1 == NULL_VAL && opCode != POP)
        return -1;
    if (expectedParamCount == 1 && src2 != NULL_VAL)
        return 1;
    if (expectedParamCount == 0 && src1 != NULL_VAL)
        return 1;
    if (src1 == NULL_VAL && src2 != NULL_VAL)
        return expectedParamCount == 0 ? 1 : -1;
    return 0;
}

bool Decode::argumentIsUndefined(byte src)
{
    return !((src >= NULL_VAL && src <= ST_SIZE) ||
        (src >= R0 && src <= R7) ||
        (src >= Z0 && src <= Z3) || 
        (src >= ADDR_R0 && src <= ADDR_R7));
}

bool Decode::argumentsAreIncompatible(byte opCode, byte src1, byte src2)
{
    bool param1IsStack = src1 >= SP_REG && src1 <= ST_SIZE;
    bool param2IsStack = src2 >= SP_REG && src2 <= ST_SIZE;
    bool param1IsZReg = src1 >= Z0 && src1 <= Z3;
    bool param2IzZReg = src2 >= Z0 && src2 <= Z3;
    
    bool immGivenAsDestination = src1 == IMM && 
        (opCode < MUL || opCode == POP);
    bool twoStackSrcsForMov = opCode == MOV && 
        param1IsStack && param2IsStack;
    bool stackSrcAnywhereOtherThanMovOrCmp = opCode != MOV && 
        opCode != CMP && (param1IsStack || param2IsStack);
    bool zRegWithForbiddenOp = (param1IsZReg || param2IsStack) && 
        !(opCode >= ADD && opCode <= DIV);
    bool arithmOpWithOnlyOneZReg = (param1IsZReg || param2IzZReg) && 
        !(param1IsZReg && param2IzZReg) &&
        (opCode == ADD || opCode == SUB || opCode == MUL || opCode == DIV);
    bool movWithZRegAndNonAddress = opCode == MOV &&
        (param1IsZReg && src2 != ADDR && !(src2 >= ADDR_R0 && src2 <= ADDR_R7)) ||
        (param2IzZReg && src1 != ADDR && !(src1 >= ADDR_R0 && src1 <= ADDR_R0));

    return immGivenAsDestination || 
        twoStackSrcsForMov || 
        stackSrcAnywhereOtherThanMovOrCmp || 
        zRegWithForbiddenOp || 
        arithmOpWithOnlyOneZReg || 
        movWithZRegAndNonAddress;
}

Instruction Decode::decodeInstructionHeader(word instruction)
{
    byte opCode = instruction >> 10;
    if (opCode == UNINITIALIZED_MEM)
        return Instruction(UNINITIALIZED_MEM);
    if (opCode == UNDEFINED || opCode > EXCP_EXIT)
        return Instruction(UNDEFINED);
    if (opCode == EXCP_EXIT && !(*flags & EXCEPTION))
        return Instruction(UNDEFINED);

    byte src1 = (instruction >> 5) & 0b11111;
    byte src2 = instruction & 0b11111;

    return Instruction(opCode, src1, src2);
}

bool Decode::processFetchWindow(fetch_window newBatch)
{
    Instruction instr = decodeInstructionHeader(word (newBatch >> ((FETCH_WINDOW_BYTES - WORD_BYTES) * 8)));
    SynchronizedDataPackage<Instruction> syncResponse(instr, cache.getAssociatedInstrAddr());
    
    if (instr.opCode == UNDEFINED)
    {
        syncResponse.exceptionTriggered = true;
        syncResponse.excpData = UNKNOWN_OP_CODE;
        syncResponse.handlerAddr = INVALID_DECODE_HANDL;
    }
    
    char argsCountDif = providedVsExpectedArgsCountDif(instr.opCode, instr.src1, instr.src2);
    if (argsCountDif != 0)
    {
        syncResponse.exceptionTriggered = true;
        syncResponse.excpData = argsCountDif < 0 ? NULL_SRC : NON_NULL_SRC;
        syncResponse.handlerAddr = INVALID_DECODE_HANDL;
    }

    if (argumentIsUndefined(instr.src1) || argumentIsUndefined(instr.src2) || argumentsAreIncompatible(instr.opCode, instr.src1, instr.src2))
    {
        syncResponse.exceptionTriggered = true;
        syncResponse.excpData = INCOMPATIBLE_PARAMS;
        syncResponse.handlerAddr = INVALID_DECODE_HANDL;
    }

    byte paramsCount = 0;
    if (instr.opCode != UNINITIALIZED_MEM && !syncResponse.exceptionTriggered)
    {
        if (instr.src1 == IMM || instr.src1 == ADDR)
        {
            syncResponse.data.param1 = newBatch >> ((FETCH_WINDOW_BYTES - WORD_BYTES * 2) * 8);
            ++paramsCount;
        }
        if (instr.src2 == IMM || instr.src2 == ADDR)
        {
            syncResponse.data.param2 = newBatch >> ((FETCH_WINDOW_BYTES - WORD_BYTES * (paramsCount == 0 ? 2 : 3)) * 8);
            ++paramsCount;
        }
    }
    cache.shiftUsedWords(paramsCount + 1);
    waitTillLastTick();
    syncResponse.sentAt = clockSyncVars->cycleCount;
    fromMetoEX->sendA(syncResponse);
    if (instr.opCode != UNINITIALIZED_MEM && !syncResponse.exceptionTriggered && clockSyncVars->running)
        logComplete(getCurrTime(), log(LoggablePackage(syncResponse.associatedIP, syncResponse.data)));
    return true;
}

void Decode::executeModuleLogic()
{
    if (fromMetoEX->pendingB())
    {
        discardUntilAddr = fromMetoEX->getB();
        if (discardUntilAddr % 2 == 1)
        {
            fromMetoEX->sendA(SynchronizedDataPackage<Instruction> (discardUntilAddr,
                MISALIGNED_IP,
                MISALIGNED_IP_HANDL));
            discardUntilAddr = DUMMY_ADDRESS;
            return;
        }
        cache.discardCurrent();
        logComplete(getCurrTime(), logJump(discardUntilAddr));
        while (fromICtoMe->pendingA() && clockSyncVars->running)
            logComplete(getCurrTime(), logDiscard(fromICtoMe->getA().associatedIP, discardUntilAddr));
        fromICtoMe->sendB(discardUntilAddr);
    }

    while (fromICtoMe->pendingA() && discardUntilAddr != DUMMY_ADDRESS && clockSyncVars->running)
    {
        SynchronizedDataPackage<fetch_window> nextBatch = fromICtoMe->getA();
        awaitNextTickToHandle(nextBatch);
        if (nextBatch.associatedIP == discardUntilAddr / FETCH_WINDOW_BYTES * FETCH_WINDOW_BYTES)
        {
            cache.overwriteCache(nextBatch.data, nextBatch.associatedIP);
            cache.shiftUsedWords((discardUntilAddr - nextBatch.associatedIP) / WORD_BYTES);
            discardUntilAddr = DUMMY_ADDRESS;
        }
        else
            logComplete(getCurrTime(), logDiscard(nextBatch.associatedIP, discardUntilAddr));
    }

    if (discardUntilAddr != DUMMY_ADDRESS)
        return;

    if (cache.canProvideFullInstruction())
    {
        processFetchWindow(cache.getFullInstrFetchWindow());
        return;
    }

    if (fromICtoMe->pendingA())
    {
        SynchronizedDataPackage<fetch_window> receivedFW = fromICtoMe->getA();
        awaitNextTickToHandle(receivedFW);
        if (cache.getStoredWordsCount() == 0)
            cache.overwriteCache(receivedFW.data, receivedFW.associatedIP);
        else
            cache.concatNewFW(receivedFW.data);
        processFetchWindow(cache.getFullInstrFetchWindow());
    }
}