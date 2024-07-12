#include "Decode.h"

Decode::Decode(std::shared_ptr<InterThreadCommPipe<address, fetch_window>> commPipeWithIC, std::shared_ptr<InterThreadCommPipe<byte, Instruction>> commPipeWithEX, std::shared_ptr<register_16b> ip):
    requestsToIC(commPipeWithIC), requestsFromEX(commPipeWithEX), IP(ip) {};

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
    if (opCode < JMP &&
        (src1 * src2 == NULL_VAL))
        return false;

    if (opCode < MUL &&
        src1 == IMM)
        return false;

    if ((opCode >= JMP && opCode <= CALL || opCode == PUSH) &&
        (src1 == NULL_VAL || src2 != NULL_VAL))
        return false;

    if ((opCode == RET || opCode == END_SIM) &&
        (src1 + src2 != NULL_VAL))
        return false;

    if (opCode == POP &&
        src1 == IMM)
        return false;

    return true;
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
    if (opCode == UNDEFINED || opCode > POP)
        throw "Unknown operation code";
    byte src1 = (instruction >> 5) & 0b11111;
    byte src2 = instruction & 0b11111;
    
    if (!argumentsMatchExpectedNumber(opCode, src1, src2))
        throw "Wrong number of arguments for this operation";
    
    if (!argumentsMatchExpectedTypes(opCode, src1, src2))
        throw "Wrong arguments' types for this operation";

    if (!argumentsAreNotMutuallyExclusive(opCode, src1, src2))
        throw "Arguments are mutually exclusive for this operation";

    return Instruction(opCode, src1, src2);
}

// TO DO: fetch window aliniat la 64b
void Decode::moveIP(byte const paramsCount)
{
    *IP += (paramsCount + 1) * 2;
}

void Decode::processFetchWindow(fetch_window newBatch)
{
    // assert
    if (*IP % 2 != 0)
        throw "IP register misaligned";

    Instruction instr = decodeInstructionHeader(word (newBatch >> 48));
    byte paramsCount = 0;
    if (instr.src1 == IMM || instr.src1 == ADDR)
    {
        instr.param1 = (newBatch << 16) >> 48;
        ++paramsCount;
    }
    if (instr.src2 == IMM || instr.src2 == ADDR)
    {
        instr.param2 = (newBatch << (paramsCount == 0 ? 16 : 32)) >> 48;
        ++paramsCount;
    }
    moveIP(paramsCount);
    requestsFromEX->sendResponse(instr);
}

void Decode::run()
{
    fetch_window newBatch;
    // TO DO: Proper loop by checking RUNNING flag
    while(*IP != 0xffff)
    {
        // TO DO: Future
        if (!requestsFromEX->pendingRequest())
            continue;
        requestsFromEX->getRequest();
        requestsToIC->sendRequest(*IP);
        // TO DO
        while(!requestsToIC->pendingResponse()) ;
        newBatch = requestsToIC->getResponse();
        processFetchWindow(newBatch);
    }
}