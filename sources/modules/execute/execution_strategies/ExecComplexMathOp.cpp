#include "ExecComplexMathOp.h"

ExecComplexMathOp::ExecComplexMathOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
    std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, address>> commPipeWithDE,
    IClockBoundModule* refToEX,
    std::shared_ptr<CPURegisters> registers):
        IExecutionStrategy(commPipeWithLS, commPipeWithDE, refToEX, registers) {};

void ExecComplexMathOp::executeInstruction(SynchronizedDataPackage<Instruction> instrPackage)
{
    Instruction instr = instrPackage.data;
    word actualParam1 = getFinalArgValue(instr.src1, instr.param1);
    word actualParam2 = getFinalArgValue(instr.src2, instr.param2);
    if (instr.opCode == MUL)
    {
        uint32_t result = ((uint32_t) actualParam1) * actualParam2;
        if (result == 0)
            *regs->flags |= ZERO;

        storeResultAtDest(result >> (8 * WORD_BYTES), R0);
        storeResultAtDest(result, R1);
        clock_time lastTick = refToEX->waitTillLastTick();
        logComplete(lastTick, log(LoggablePackage(instr, result >> (8 * WORD_BYTES), result)));
    }
    else
    {
        if(actualParam2 == 0)
        {
            instrPackage.exceptionTriggered = true;
            instrPackage.handlerAddr = 0x0000;
            instrPackage.excpData = 0x0000;
            handleException(instrPackage);
            return;
        }

        word ratio = actualParam1 / actualParam2;
        word modulus = actualParam1 % actualParam2;
        if (ratio == 0 && modulus == 0)
            *regs->flags |= ZERO;
        storeResultAtDest(ratio, R0);
        storeResultAtDest(modulus, R1);
        clock_time lastTick = refToEX->waitTillLastTick();
        logComplete(lastTick, log(LoggablePackage(instr, ratio, modulus)));
    }
    moveIP(instr);
}


std::string ExecComplexMathOp::log(LoggablePackage toLog)
{
    std::string result = plainInstructionToString(toLog.instr) + " (r0 = ";
    result += std::to_string(toLog.actualParam1) + ", r1 = " + std::to_string(toLog.actualParam2) + ")";
    if (toLog.actualParam1 == 0 && toLog.actualParam2 == 0)
        result += " Flags.Z=1";
    result += "\n";
    return result;
}