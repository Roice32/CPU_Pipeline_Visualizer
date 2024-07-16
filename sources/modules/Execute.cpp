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

Execute::Execute(std::shared_ptr<InterThreadCommPipe<MemoryAccessRequest, word>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<address, Instruction>> commPipeWithDE,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
        IClockBoundModule(clockSyncVars, 5, "Execute"), 
        requestsToLS(commPipeWithLS), requestsToDE(commPipeWithDE), registers(registers)
{
    std::shared_ptr<ExecSimpleMathOp> addOrSub = std::make_shared<ExecSimpleMathOp>(commPipeWithLS, registers);
    execStrategies.insert({ADD, addOrSub});
    execStrategies.insert({SUB, addOrSub});
    std::shared_ptr<ExecComplexMathOp> mulOrDiv = std::make_shared<ExecComplexMathOp>(commPipeWithLS, registers);
    execStrategies.insert({MUL, mulOrDiv});
    execStrategies.insert({DIV, mulOrDiv});
    std::shared_ptr<ExecMov> mov = std::make_shared<ExecMov>(commPipeWithLS, registers);
    execStrategies.insert({MOV, mov});
    std::shared_ptr<ExecCmp> cmp = std::make_shared<ExecCmp>(commPipeWithLS, registers);
    execStrategies.insert({CMP, cmp});
    std::shared_ptr<ExecJumpOp> jumpOp = std::make_shared<ExecJumpOp>(commPipeWithLS, registers);
    execStrategies.insert({JMP, jumpOp});
    execStrategies.insert({JE, jumpOp});
    execStrategies.insert({JL, jumpOp});
    execStrategies.insert({JG, jumpOp});
    execStrategies.insert({JZ, jumpOp});
    std::shared_ptr<ExecPush> push = std::make_shared<ExecPush>(commPipeWithLS, registers);
    execStrategies.insert({PUSH, push});
    std::shared_ptr<ExecCall> call = std::make_shared<ExecCall>(commPipeWithLS, registers, push);
    execStrategies.insert({CALL, call});
    std::shared_ptr<ExecPop> pop = std::make_shared<ExecPop>(commPipeWithLS, registers);
    execStrategies.insert({POP, pop});
    std::shared_ptr<ExecRet> ret = std::make_shared<ExecRet>(commPipeWithLS, registers, pop);
    execStrategies.insert({RET, ret});
    std::shared_ptr<ExecEndSim> endSim = std::make_shared<ExecEndSim>(commPipeWithLS, registers);
    execStrategies.insert({END_SIM, endSim});
};

void Execute::executeInstruction(Instruction instr)
{
    auto foundStrategy = execStrategies.find((OpCode) instr.opCode); 
    assert(foundStrategy != execStrategies.end() && "Undefined instruction");
    foundStrategy->second->executeInstruction(instr);
}

bool Execute::executeModuleLogic()
{
    // TO DO: If no instruction queued, return false.
    // ALSO, lad is 1 tick short but maybe when he won't be making requests that will get fixed.
    requestsToDE->sendRequest(*registers->IP);
    enterIdlingState();
    while (!requestsToDE->pendingResponse()) ;
    Instruction currInstr = requestsToDE->getResponse();
    returnFromIdlingState();
    executeInstruction(currInstr);
    waitTillLastTick();
    return true;
}