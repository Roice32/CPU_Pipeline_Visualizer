#include "Execute.h"
#include "Config.h"
#include <cstdio>

Execute::Execute(LoadStore* lsModule, CPURegisters* registers, InstructionCache* icModule): IMemoryAccesser(lsModule)
{
    this->registers = registers;
    ICModule = icModule;
    execStrategies = new ExecutionStrategyPool(lsModule, icModule, registers);
}

word Execute::requestDataAt(word addr)
{
    word result = 0;
    result = LSModule->loadFrom(addr);
    result <<= 8;
    result |= LSModule->loadFrom(addr + 8);
    return result;
}

void Execute::storeDataAt(word addr, word data)
{
    LSModule->storeAt(addr, data >> 8);
    LSModule->storeAt(addr + 8, (data << 8) >> 8);
}

void Execute::executeInstruction(Instruction instr)
{
    printf("Instruction: %hu %hu %hu %hu %hu\n", instr.opCode, instr.src1, instr.src2, instr.param1, instr.param2);
    switch (instr.opCode)
    {
        case ADD: case SUB:
            execStrategies->addOrSub->executeInstruction(instr);
        break;
        case MOV:
            execStrategies->mov->executeInstruction(instr);
        break;
        case MUL: case DIV:
            execStrategies->mulOrDiv->executeInstruction(instr);
        break;
        case CMP:
            execStrategies->cmp->executeInstruction(instr);
        break;
        case JMP: case JE: case JL: case JG: case JZ:
            execStrategies->jmpOp->executeInstruction(instr);
        break;
        case CALL:
            // WIP
        break;
        case RET:
            // WIP
        break; 
        case END_SIM:
            registers->IP = 0xffff;
        break;
        case PUSH:
            execStrategies->push->executeInstruction(instr);
        break;
        case POP:
            // WIP
        break;
        default:
            throw "Undefined instruction";
    }

    ICModule->requestFetchWindow();
}