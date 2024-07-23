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

bool Decode::argumentsMatchExpectedTypes(byte opCode, byte src1, byte src2)
{
    bool missingEitherMandatoryParam = opCode < JMP && (src1 * src2 == NULL_VAL);
    bool immGivenAsDestination = (opCode < MUL || opCode == POP) && src1 == IMM;
    bool missingOrExtraParamForSingleParamOp = (opCode >= JMP && opCode <= CALL || opCode == PUSH) && (src1 == NULL_VAL || src2 != NULL_VAL);
    bool parameterWhereNoneNeeded = (opCode == RET || opCode == END_SIM) && (src1 + src2 != NULL_VAL);

    return !missingEitherMandatoryParam && !immGivenAsDestination && !missingOrExtraParamForSingleParamOp && !parameterWhereNoneNeeded;
}

bool Decode::argumentsAreNotMutuallyExclusive(byte opCode, byte src1, byte src2)
{
    if (opCode == MOV &&
        (src1 >= SP_REG && src1 <= ST_SIZE &&
            src2 >= SP_REG && src2 <= ST_SIZE))
        return false;
    return true;
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

    //assert(argumentsMatchExpectedTypes(opCode, src1, src2) && "Wrong arguments' types for this operation");
    assert(argumentsAreNotMutuallyExclusive(opCode, src1, src2) && "Arguments are mutually exclusive for this operation");

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

bool Decode::executeModuleLogic()
{
    if (fromMetoEX->pendingB())
    {
        discardUntilAddr = fromMetoEX->getB();
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
        return false;

    if (cache.canProvideFullInstruction())
        return processFetchWindow(cache.getFullInstrFetchWindow());
    
    if (fromICtoMe->pendingA())
    {
        SynchronizedDataPackage<fetch_window> receivedFW = fromICtoMe->getA();
        awaitNextTickToHandle(receivedFW);
        if (cache.getStoredWordsCount() == 0)
            cache.overwriteCache(receivedFW.data, receivedFW.associatedIP);
        else
            cache.concatNewFW(receivedFW.data);
        return processFetchWindow(cache.getFullInstrFetchWindow());
    }
    else
        return false;
}