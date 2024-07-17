#include "Decode.h"
#include <cassert>

Decode::Decode(std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithIC,
    std::shared_ptr<InterThreadCommPipe<address, Instruction>> commPipeWithEX,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
        IClockBoundModule(clockSyncVars, 2, "Decode"),
        requestsToIC(commPipeWithIC), requestsFromEX(commPipeWithEX) {};

byte Decode::getExpectedParamCount(byte opCode)
{
    if (opCode < JMP)
        return 2;
    if (opCode == RET || opCode == END_SIM)
        return 0;
    return 1;
}

bool Decode::argumentsMatchExpectedNumber(byte opCode, byte src1, byte src2)
{
    byte expectedParamCount = getExpectedParamCount(opCode);
    if (expectedParamCount == 2 && src2 == 0)
        return false;
    if (expectedParamCount == 1 && src1 == 0 && opCode != POP)
        return false;
    return true;
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
    assert((opCode != UNDEFINED && opCode <= POP) && "Unknown operation code");

    byte src1 = (instruction >> 5) & 0b11111;
    byte src2 = instruction & 0b11111;
    
    assert(argumentsMatchExpectedNumber(opCode, src1, src2) && "Wrong number of arguments for this operation");
    assert(argumentsMatchExpectedTypes(opCode, src1, src2) && "Wrong arguments' types for this operation");
    assert(argumentsAreNotMutuallyExclusive(opCode, src1, src2) && "Arguments are mutually exclusive for this operation");

    return Instruction(opCode, src1, src2);
}

void Decode::processFetchWindow(fetch_window newBatch)
{
    Instruction instr = decodeInstructionHeader(word (newBatch >> ((FETCH_WINDOW_BYTES - WORD_BYTES) * 8)));
    byte paramsCount = 0;
    if (instr.src1 == IMM || instr.src1 == ADDR)
    {
        instr.param1 = newBatch >> ((FETCH_WINDOW_BYTES - WORD_BYTES * 2) * 8);
        ++paramsCount;
    }
    if (instr.src2 == IMM || instr.src2 == ADDR)
    {
        instr.param2 = newBatch >> ((FETCH_WINDOW_BYTES - WORD_BYTES * (paramsCount == 0 ? 2 : 3)) * 8);
        ++paramsCount;
    }
    cache.shiftUsedWords(paramsCount + 1);
    waitTillLastTick();
    requestsFromEX->sendResponse(instr);
}

void Decode::manageCacheForRequest(address req)
{
    if (!cache.reqIPAlreadyCached(req))
    {
        requestsToIC->sendRequest(req / FETCH_WINDOW_BYTES * FETCH_WINDOW_BYTES);
        enterIdlingState();
        while(!requestsToIC->pendingResponse() && clockSyncVars->running) ;
        returnFromIdlingState();
        cache.overwriteCache(requestsToIC->getResponse(), req / 8 * 8);
    }
    cache.bringIPToStart(req);
    if (!cache.canProvideFullInstruction())
    {
        requestsToIC->sendRequest((req / FETCH_WINDOW_BYTES + 1) * FETCH_WINDOW_BYTES);
        enterIdlingState();
        while (!requestsToIC->pendingResponse() && clockSyncVars->running) ;
        returnFromIdlingState();
        cache.concatNewFW(requestsToIC->getResponse());
    }
}

bool Decode::executeModuleLogic()
{
    if (!requestsFromEX->pendingRequest())
        return false;
    address currReq;
    currReq = requestsFromEX->getRequest();
    manageCacheForRequest(currReq);
    processFetchWindow(cache.getFullInstrFetchWindow());
    return true;
}