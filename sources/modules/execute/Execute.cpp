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

Execute::Execute(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
        IClockBoundModule(clockSyncVars, 5, "Execute"), 
        fromMeToLS(commPipeWithLS), fromDEtoMe(commPipeWithDE), registers(registers)
{
    std::shared_ptr<ExecSimpleMathOp> addOrSub = std::make_shared<ExecSimpleMathOp>(commPipeWithLS, this, registers);
    execStrategies.insert({ADD, addOrSub});
    execStrategies.insert({SUB, addOrSub});
    std::shared_ptr<ExecComplexMathOp> mulOrDiv = std::make_shared<ExecComplexMathOp>(commPipeWithLS, this, registers);
    execStrategies.insert({MUL, mulOrDiv});
    execStrategies.insert({DIV, mulOrDiv});
    std::shared_ptr<ExecMov> mov = std::make_shared<ExecMov>(commPipeWithLS, this, registers);
    execStrategies.insert({MOV, mov});
    std::shared_ptr<ExecCmp> cmp = std::make_shared<ExecCmp>(commPipeWithLS, this, registers);
    execStrategies.insert({CMP, cmp});
    std::shared_ptr<ExecJumpOp> jumpOp = std::make_shared<ExecJumpOp>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({JMP, jumpOp});
    execStrategies.insert({JE, jumpOp});
    execStrategies.insert({JL, jumpOp});
    execStrategies.insert({JG, jumpOp});
    execStrategies.insert({JZ, jumpOp});
    std::shared_ptr<ExecPush> push = std::make_shared<ExecPush>(commPipeWithLS, this, registers);
    execStrategies.insert({PUSH, push});
    std::shared_ptr<ExecCall> call = std::make_shared<ExecCall>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({CALL, call});
    std::shared_ptr<ExecPop> pop = std::make_shared<ExecPop>(commPipeWithLS, this, registers);
    execStrategies.insert({POP, pop});
    std::shared_ptr<ExecRet> ret = std::make_shared<ExecRet>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({RET, ret});
    std::shared_ptr<ExecEndSim> endSim = std::make_shared<ExecEndSim>(commPipeWithLS, this, registers, clockSyncVars);
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
    if (!fromDEtoMe->pendingA())
        return false;

    SynchronizedDataPackage<Instruction> currInstr;
    do
    {
        currInstr = fromDEtoMe->getA();
    } while (fromDEtoMe->pendingA() && currInstr.associatedIP != *registers->IP);

    if (currInstr.associatedIP != *registers->IP)
        return false;
    
    // TO DO: Handle UNDEFINED & UNINITIALIZED_MEM delivered by DE differently?
    if (currInstr.data.opCode == UNINITIALIZED_MEM || currInstr.data.opCode == UNDEFINED)
        return false;

    awaitNextTickToHandle(currInstr);
    executeInstruction(currInstr.data);
    return true;
}