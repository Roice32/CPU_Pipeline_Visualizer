#include "Execute.h"
#include "Config.h"

Execute::Execute(LoadStore* lsModule, CPURegisters* registers, InstructionCache* icModule):
    registers(registers), ICModule(icModule), execStrategies(new ExecutionStrategyPool(lsModule, icModule, registers)) {};

void Execute::executeInstruction(Instruction instr)
{
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
            execStrategies->call->executeInstruction(instr);
        break;
        case RET:
            execStrategies->ret->executeInstruction(instr);
        break; 
        case END_SIM:
            registers->IP = 0xffff; // WIP
        break;
        case PUSH:
            execStrategies->push->executeInstruction(instr);
        break;
        case POP:
            execStrategies->pop->executeInstruction(instr);
        break;
        default:
            throw "Undefined instruction";
    }

    ICModule->requestFetchWindow();
}