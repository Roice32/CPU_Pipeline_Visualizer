#include "EXLogger.h"

EXLogger::EXLogger(): ILogger("EX")
{
    if (opNames.empty())
    {
        opNames.insert({ADD, "add"});
        opNames.insert({SUB, "sub"});
        opNames.insert({MOV, "mov"});
        opNames.insert({MUL, "mul"});
        opNames.insert({DIV, "div"});
        opNames.insert({CMP, "cmp"});
        opNames.insert({JMP, "jmp"});
        opNames.insert({JE, "je"});
        opNames.insert({JL, "jl"});
        opNames.insert({JG, "jg"});
        opNames.insert({JZ, "jz"});
        opNames.insert({CALL, "call"});
        opNames.insert({RET, "ret"});
        opNames.insert({END_SIM, "end_sim"});
        opNames.insert({PUSH, "push"});
        opNames.insert({POP, "pop"});
    }
    if (typeNames.empty())
    {
        typeNames.insert({NULL_VAL, ""});
        typeNames.insert({SP_REG, "sp"});
        typeNames.insert({ST_BASE, "stack_base"});
        typeNames.insert({ST_SIZE, "stack_size"});
        typeNames.insert({R0, "r0"});
        typeNames.insert({R1, "r1"});
        typeNames.insert({R2, "r2"});
        typeNames.insert({R3, "r3"});
        typeNames.insert({R4, "r4"});
        typeNames.insert({R5, "r5"});
        typeNames.insert({R6, "r6"});
        typeNames.insert({R7, "r7"});
        typeNames.insert({ADDR_R0, "[r0]"});
        typeNames.insert({ADDR_R1, "[r1]"});
        typeNames.insert({ADDR_R2, "[r2]"});
        typeNames.insert({ADDR_R3, "[r3]"});
        typeNames.insert({ADDR_R4, "[r4]"});
        typeNames.insert({ADDR_R5, "[r5]"});
        typeNames.insert({ADDR_R6, "[r6]"});
        typeNames.insert({ADDR_R7, "[r7]"});
        // If registers' count ever changes, this needs changing, too.
    }
}

bool EXLogger::mustDisplayParamValue(byte src)
{
    return src == IMM || src == ADDR;
}

bool EXLogger::canBeReplacedByValue(byte src)
{
    return src != NULL_VAL && src != IMM;
}

void EXLogger::printPlainArg(byte src, word param, bool spaced)
{
    if (spaced)
        printf(" ");
    if (mustDisplayParamValue(src))
    {
        if (src == IMM)
            printf("%hu", param);
        else
            printf("[%hu]", param);
    }
    else
        printf("%s", typeNames.at((TypeCode) src));
}

void EXLogger::printPlainInstruction(Instruction instr)
{
    printf(" %s", opNames.at((OpCode) instr.opCode));
    printPlainArg(instr.src1, instr.param1);
    if (instr.src1 * instr.src2 != NULL_VAL)
        printf(",");
    printPlainArg(instr.src2, instr.param2);
}

void EXLogger::printInstructionWithParamsReplaced(Instruction instr, word actualParam1, word actualParam2)
{
    printf(" (%s", opNames.at((OpCode) instr.opCode));
    if (canBeReplacedByValue(instr.src1) && instr.opCode != MOV)
        printf(" %hu", actualParam1);
    else
        printPlainArg(instr.src1, instr.param1);

    if (instr.src1 * instr.src2 != NULL_VAL)
        printf(",");
    
    if (canBeReplacedByValue(instr.src2))
        printf(" %hu", actualParam2);
    else
        printPlainArg(instr.src2, instr.param2);
    printf(")");
}

void EXLogger::log(LoggablePackage toLog)
{
    printPlainInstruction(toLog.ex.instr);
    if (canBeReplacedByValue(toLog.ex.instr.src1) && toLog.ex.instr.opCode != MOV || canBeReplacedByValue(toLog.ex.instr.src2))
        printInstructionWithParamsReplaced(toLog.ex.instr, toLog.ex.actualParam1, toLog.ex.actualParam2);
    if (toLog.ex.newLine)
        printf("\n");
}

void EXLogger::printFlagsChange(register_16b oldFlagsState, register_16b newFlagsState, bool initSpace)
{
    if (oldFlagsState == newFlagsState)
        return;
    if (initSpace)
        printf(" ");
    if ((oldFlagsState & ZERO) != (newFlagsState & ZERO))
        printf("Flags.Z=%c ", (newFlagsState & ZERO) ? '1' : '0');
    if ((oldFlagsState & EQUAL) != (newFlagsState & EQUAL))
        printf("Flags.E=%c ", (newFlagsState & EQUAL) ? '1' : '0');
    if ((oldFlagsState & GREATER) != (newFlagsState & GREATER))
        printf("Flags.G=%c ", (newFlagsState & GREATER) ? '1' : '0');
}