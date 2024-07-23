#include "ExceptionHandler.h"

ExceptionHandler::ExceptionHandler(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, refToEX, registers), fromDEtoMe(commPipeWithDE) {};

std::string ExceptionHandler::logReceived(SynchronizedDataPackage<Instruction> faultyInstr)
{
    std::string result = "Encountered exception ";
    if (faultyInstr.handlerAddr == DIV_BY_ZERO_HANDL)
        result += "'Division by 0'";
    if (faultyInstr.handlerAddr == INVALID_DECODE_HANDL)
    {
        if (faultyInstr.excpData == UNKNOWN_OP_CODE)
            result += "'Unknown operation code'";
        else if (faultyInstr.excpData == NULL_SRC)
            result += "'Null where argument expected'";
        else if (faultyInstr.excpData == NON_NULL_SRC)
            result += "'Argument where null expected'";
        else
            result += "'Incompatible parameters (mutually / for given operation)'";
    }
    if (faultyInstr.handlerAddr == MISALIGNED_ACCESS_HANDL)
        result += "'Request to memory address not aligned to 16b'";
    if (faultyInstr.handlerAddr == STACK_OVERFLOW_HANDL)
    {
        if (faultyInstr.excpData == PUSH_OVERFLOW)
            result += "'Over-pushed stack exceeded upper limit'";
        else
            result += "'Over-popped stack exceeded lower limit'";
    }
    if (faultyInstr.handlerAddr == MISALIGNED_IP_HANDL)
        result += "'IP not aligned to 16b'";

    return result;
}

void ExceptionHandler::handleException(SynchronizedDataPackage<Instruction> faultyInstr)
{
    logComplete(refToEX->getCurrTime(), logReceived(faultyInstr));
    if (*regs->flags & EXCEPTION)
    {
        refToEX->endSimulation();
        std::string endMessage = "\t!EX forcefully ends simulation at T=" + std::to_string(refToEX->getCurrTime()) + " due to double exception!\n";
        logAdditional(endMessage);
        return;
    }

    *regs->flags |= EXCEPTION;

    assert((*regs->stackPointer >= (REGISTER_COUNT + 4) * WORD_BYTES) && "Insufficient stack space for exception handler call");
    word methodAddress = requestDataAt(faultyInstr.handlerAddr, 1)[0];
    
    std::vector<word> savedState;
    savedState.push_back(faultyInstr.associatedIP);
    savedState.push_back(*regs->stackPointer);
    savedState.push_back(*regs->flags);
    savedState.push_back(faultyInstr.excpData);
    for (byte reg = 0; reg < REGISTER_COUNT; ++reg)
        savedState.push_back(*regs->registers[reg]);
    storeDataAt(SAVE_STATE_ADDR, REGISTER_COUNT + 4, savedState);
    
    std::string message = "Calling exception handler at #" + convDecToHex(methodAddress) + "\n";
    clock_time lastTick = refToEX->waitTillLastTick();
    *regs->IP = methodAddress;
    fromDEtoMe->sendB(methodAddress);
    logComplete(refToEX->getCurrTime(), message);
}

void ExceptionHandler::executeInstruction(Instruction instr) {};

std::string ExceptionHandler::log(LoggablePackage toLog)
    { return ""; }