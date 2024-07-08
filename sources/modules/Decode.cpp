#include "Decode.h"

byte Decode::getExpectedParamCount(byte opCode)
{
    if (opCode < 7)
        return 2;
    if (opCode == 14 || opCode == 15)
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
    
    if (opCode == MOV &&
        (src1 >= SP_REG && src1 <= ST_SIZE &&
            src2 >= SP_REG && src2 <= ST_SIZE))
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

Instruction Decode::decodeInstructionHeader(word instruction)
{
    byte opCode = instruction >> 10;
    if (opCode == 8 || opCode > POP)
        throw "Unknown operation code";
    byte src1 = (instruction >> 5) & 0b11111;
    byte src2 = instruction & 0b11111;
    
    if (!argumentsMatchExpectedNumber(opCode, src1, src2))
        throw "Wrong arguments count";
    
    if (!argumentsMatchExpectedTypes(opCode, src1, src2))
        throw "Wrong arguments types";
    
    // Also check for incompatible arguments

    return Instruction(opCode, src1, src2);
}

void Decode::moveIP(byte const paramsCount)
{
    *IP += (paramsCount + 1) * 2;
}

void Decode::processFetchWindow(fetch_window newBatch)
{
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
    EXModule->executeInstruction(instr);
}

void Decode::setEXModule(Execute* exModuleRef)
{
    EXModule = exModuleRef;
}