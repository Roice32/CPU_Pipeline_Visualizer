#include "Execute.h"
#include "ExecGather.h"
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
#include "ExecScatter.h"

Execute::Execute(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
  std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
  std::shared_ptr<CPURegisters> registers,
  std::shared_ptr<ClockSyncPackage> clockSyncVars,
  std::shared_ptr<ExecutionRecorder> recorder) :
    IClockBoundModule(clockSyncVars, EX_CYCLES_PER_OP),
    fromMeToLS(commPipeWithLS),
    fromDEtoMe(commPipeWithDE),
    registers(registers),
    exceptionHandler(GeneralExceptionHandler(commPipeWithLS, commPipeWithDE, this, registers, recorder)),
    recorder(recorder)
{
  std::shared_ptr<ExecSimpleMathOp> addOrSub = std::make_shared<ExecSimpleMathOp>(commPipeWithLS,
                                                                                  commPipeWithDE,
                                                                                  this,
                                                                                  registers,
                                                                                  recorder);
  execStrategies.insert({ADD, addOrSub});
  execStrategies.insert({SUB, addOrSub});

  std::shared_ptr<ExecComplexMathOp> mulOrDiv = std::make_shared<ExecComplexMathOp>(commPipeWithLS,
                                                                                    commPipeWithDE,
                                                                                    this,
                                                                                    registers,
                                                                                    recorder);
  execStrategies.insert({MUL, mulOrDiv});
  execStrategies.insert({DIV, mulOrDiv});

  std::shared_ptr<ExecMov> mov = std::make_shared<ExecMov>(commPipeWithLS,
                                                           commPipeWithDE,
                                                           this,
                                                           registers,
                                                           recorder);
  execStrategies.insert({MOV, mov});

  std::shared_ptr<ExecCmp> cmp = std::make_shared<ExecCmp>(commPipeWithLS,
                                                           commPipeWithDE,
                                                           this,
                                                           registers,
                                                           recorder);
  execStrategies.insert({CMP, cmp});

  std::shared_ptr<ExecJumpOp> jumpOp = std::make_shared<ExecJumpOp>(commPipeWithLS,
                                                                    commPipeWithDE,
                                                                    this,
                                                                    registers,
                                                                    recorder);
  execStrategies.insert({JMP, jumpOp});
  execStrategies.insert({JE, jumpOp});
  execStrategies.insert({JL, jumpOp});
  execStrategies.insert({JG, jumpOp});
  execStrategies.insert({JZ, jumpOp});

  std::shared_ptr<ExecPush> push = std::make_shared<ExecPush>(commPipeWithLS,
                                                              commPipeWithDE,
                                                              this,
                                                              registers,
                                                              recorder);
  execStrategies.insert({PUSH, push});

  std::shared_ptr<ExecCall> call = std::make_shared<ExecCall>(commPipeWithLS,
                                                              commPipeWithDE,
                                                              this,
                                                              registers,
                                                              recorder);
  execStrategies.insert({CALL, call});

  std::shared_ptr<ExecPop> pop = std::make_shared<ExecPop>(commPipeWithLS,
                                                           commPipeWithDE,
                                                           this,
                                                           registers,
                                                           recorder);
  execStrategies.insert({POP, pop});

  std::shared_ptr<ExecRet> ret = std::make_shared<ExecRet>(commPipeWithLS,
                                                           commPipeWithDE,
                                                           this,
                                                           registers,
                                                           recorder);
  execStrategies.insert({RET, ret});

  std::shared_ptr<ExecEndSim> endSim = std::make_shared<ExecEndSim>(commPipeWithLS,
                                                                    commPipeWithDE,
                                                                    this,
                                                                    registers,
                                                                    recorder);
  execStrategies.insert({END_SIM, endSim});

  std::shared_ptr<ExecExcpExit> excpExit = std::make_shared<ExecExcpExit>(commPipeWithLS,
                                                                          commPipeWithDE,
                                                                          this,
                                                                          registers,
                                                                          recorder);
  execStrategies.insert({EXCP_EXIT, excpExit});

  std::shared_ptr<ExecGather> gather = std::make_shared<ExecGather>(commPipeWithLS,
                                                                    commPipeWithDE,
                                                                    this,
                                                                    registers,
                                                                    recorder);
  execStrategies.insert({GATHER, gather});

  std::shared_ptr<ExecScatter> scatter = std::make_shared<ExecScatter>(commPipeWithLS,
                                                                       commPipeWithDE,
                                                                       this,
                                                                       registers,
                                                                       recorder);
  execStrategies.insert({SCATTER, scatter});
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
  recorder->popPipeData(DEtoEX);
  bool discardHappened = false;
  while(currInstr.associatedIP != *registers->IP)
  {
    discardHappened = true;
    if (currInstr.data.opCode == UNINITIALIZED_MEM || currInstr.data.opCode == UNDEFINED)
      recorder->addExtraInfo(EX, "Discarded malformed / residual instruction at #"
                               + convDecToHex(currInstr.associatedIP));
    else
      recorder->addExtraInfo(EX, "Discarded instruction at #"
                                 + convDecToHex(currInstr.associatedIP));
    if (fromDEtoMe->pendingA())
    {
      currInstr = fromDEtoMe->getA();
      recorder->popPipeData(DEtoEX);
    }
    else
      break;
  }
  if (discardHappened)
    recorder->addExtraInfo(EX, "(Awaiting for instruction at #"
                               + convDecToHex(*registers->IP)
                               + ")");

  if (currInstr.exceptionTriggered && currInstr.excpData == MISALIGNED_IP)
  {
    exceptionHandler.handleException(currInstr);
    return;
  }

  if (currInstr.data.opCode == UNINITIALIZED_MEM)
  {
    if (currInstr.associatedIP == *registers->IP)
      *registers->IP += 2;
    recorder->modifyModuleState(EX, "Awaiting for instruction at #"
                                    + convDecToHex(*registers->IP));

    return;
  }

  if (currInstr.associatedIP != *registers->IP)
    return;

  awaitNextTickToHandle(currInstr);
  if (currInstr.exceptionTriggered)
    exceptionHandler.handleException(currInstr);
  else
  {
    recorder->modifyModuleState(EX, "Began executing instruction '"
                                    + currInstr.data.toString() + "' from #"
                                    + convDecToHex(currInstr.associatedIP));
    executeInstruction(currInstr);
    recorder->addExtraInfo(EX, "Retired instruction");
  }
}