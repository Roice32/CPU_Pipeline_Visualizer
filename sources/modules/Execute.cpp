#include "Execute.h"
#include "Config.h"
#include <cstdio>

Execute::Execute(LoadStore* lsModule, CPURegisters* registers, InstructionCache* icModule): IMemoryAccesser(lsModule)
{
    this->registers = registers;
    ICModule = icModule;
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
        case ADD:
            
        break;
        case SUB:
            // WIP
        break;
        case MOV:
            // WIP
        break;
        case MUL:
            // WIP
        break;
        case DIV:
            // WIP
        break;
        case CMP:
            // WIP
        break;
        case JMP:
            // Made to work only for jumps to labels fttb
            registers->IP = instr.param1;
        break;
        case JE:
            // WIP
        break;
        case JL:
            // WIP
        break;
        case JG:
            // WIP
        break;
        case JZ:
            // WIP
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
            // WIP
        break;
        case POP:
            // WIP
        break;
        default:
            throw "Undefined instruction";
    }

    ICModule->requestFetchWindow();
}