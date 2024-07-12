#include "Execute.h"
#include "ExecSimpleMathOp.h"
#include "ExecComplexMathOp.h"
#include "ExecMov.h"
#include "ExecCmp.h"
#include "ExecJumpOp.h"
#include "ExecCall.h"
#include "ExecRet.h"
#include "ExecEndSim.h"
#include "ExecPush.h"
#include "ExecPop.h"
#include "Config.h"

Execute::Execute(LoadStore* lsModule, CPURegisters* registers, InstructionCache* icModule):
    registers(registers), ICModule(icModule)
{
    ExecSimpleMathOp* addOrSub = new ExecSimpleMathOp(lsModule, icModule, registers);
    execStrategies.insert({ADD, addOrSub});
    execStrategies.insert({SUB, addOrSub});
    ExecComplexMathOp* mulOrDiv = new ExecComplexMathOp(lsModule, icModule, registers);
    execStrategies.insert({MUL, mulOrDiv});
    execStrategies.insert({DIV, mulOrDiv});
    ExecMov* mov = new ExecMov(lsModule, icModule, registers);
    execStrategies.insert({MOV, mov});
    ExecCmp* cmp = new ExecCmp(lsModule, icModule, registers);
    execStrategies.insert({CMP, cmp});
    ExecJumpOp* jumpOp = new ExecJumpOp(lsModule, icModule, registers);
    execStrategies.insert({JMP, jumpOp});
    execStrategies.insert({JE, jumpOp});
    execStrategies.insert({JL, jumpOp});
    execStrategies.insert({JG, jumpOp});
    execStrategies.insert({JZ, jumpOp});
    ExecPush* push = new ExecPush(lsModule, icModule, registers);
    execStrategies.insert({PUSH, push});
    ExecCall* call = new ExecCall(lsModule, icModule, registers, push);
    execStrategies.insert({CALL, call});
    ExecPop* pop = new ExecPop(lsModule, icModule, registers);
    execStrategies.insert({POP, pop});
    ExecRet* ret = new ExecRet(lsModule, icModule, registers, pop);
    execStrategies.insert({RET, ret});
    ExecEndSim* endSim = new ExecEndSim(lsModule, icModule, registers);
    execStrategies.insert({END_SIM, endSim});
};

void Execute::executeInstruction(Instruction instr)
{
    auto foundStrategy = execStrategies.find((OpCode) instr.opCode); 
    if (foundStrategy == execStrategies.end())
        throw "Undefined instruction";
    foundStrategy->second->executeInstruction(instr);
}

Execute::~Execute()
{
    for (byte op = ADD; op <= POP; ++op)
    {
        if (op == SUB || op == DIV || (op >= JE && op <= JZ) || op == UNDEFINED)
            continue;
        delete execStrategies.at((OpCode) op);
    }
}