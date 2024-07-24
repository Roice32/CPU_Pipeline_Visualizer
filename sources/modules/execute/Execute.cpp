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
#include "ExecExcpExit.h"

Execute::Execute(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    std::shared_ptr<CPURegisters> registers,
    std::shared_ptr<ClockSyncPackage> clockSyncVars):
        IClockBoundModule(clockSyncVars, 5, "Execute"), EXLogger(),
        fromMeToLS(commPipeWithLS), fromDEtoMe(commPipeWithDE), registers(registers), exceptionHandler(GeneralExceptionHandler(commPipeWithLS, commPipeWithDE, this, registers))
{
    std::shared_ptr<ExecSimpleMathOp> addOrSub = std::make_shared<ExecSimpleMathOp>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({ADD, addOrSub});
    execStrategies.insert({SUB, addOrSub});
    std::shared_ptr<ExecComplexMathOp> mulOrDiv = std::make_shared<ExecComplexMathOp>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({MUL, mulOrDiv});
    execStrategies.insert({DIV, mulOrDiv});
    std::shared_ptr<ExecMov> mov = std::make_shared<ExecMov>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({MOV, mov});
    std::shared_ptr<ExecCmp> cmp = std::make_shared<ExecCmp>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({CMP, cmp});
    std::shared_ptr<ExecJumpOp> jumpOp = std::make_shared<ExecJumpOp>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({JMP, jumpOp});
    execStrategies.insert({JE, jumpOp});
    execStrategies.insert({JL, jumpOp});
    execStrategies.insert({JG, jumpOp});
    execStrategies.insert({JZ, jumpOp});
    std::shared_ptr<ExecPush> push = std::make_shared<ExecPush>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({PUSH, push});
    std::shared_ptr<ExecCall> call = std::make_shared<ExecCall>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({CALL, call});
    std::shared_ptr<ExecPop> pop = std::make_shared<ExecPop>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({POP, pop});
    std::shared_ptr<ExecRet> ret = std::make_shared<ExecRet>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({RET, ret});
    std::shared_ptr<ExecEndSim> endSim = std::make_shared<ExecEndSim>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({END_SIM, endSim});
    std::shared_ptr<ExecExcpExit> excpExit = std::make_shared<ExecExcpExit>(commPipeWithLS, commPipeWithDE, this, registers);
    execStrategies.insert({EXCP_EXIT, excpExit});
};

void Execute::executeInstruction(SynchronizedDataPackage<Instruction> instr)
{
    auto foundStrategy = execStrategies.find((OpCode) instr.data.opCode);
    foundStrategy->second->executeInstruction(instr);
}

void Execute::executeModuleLogic()
{
    if (!fromDEtoMe->pendingA())
        return;

    SynchronizedDataPackage<Instruction> currInstr = fromDEtoMe->getA();
    while(currInstr.associatedIP != *registers->IP)
    {
        logComplete(getCurrTime(), logDiscard(currInstr.data, currInstr.associatedIP, *registers->IP));
        if (fromDEtoMe->pendingA())
            currInstr = fromDEtoMe->getA();
        else
            break;
    }
    
    if (currInstr.exceptionTriggered && currInstr.excpData == MISALIGNED_IP)
    {
        exceptionHandler.handleException(currInstr);
        return;
    }

    if (currInstr.data.opCode == UNINITIALIZED_MEM)
    {
        if (currInstr.associatedIP == *registers->IP)
            *registers->IP += 2;
        logComplete(getCurrTime(), logSkip(currInstr.associatedIP, *registers->IP));
        return;
    }

    if (currInstr.associatedIP != *registers->IP)
        return;

    awaitNextTickToHandle(currInstr);
    if (currInstr.exceptionTriggered)
        exceptionHandler.handleException(currInstr);
    else
    {
        logComplete(getCurrTime(), logAccept(currInstr.data, *registers->IP));
        executeInstruction(currInstr);
    }
}