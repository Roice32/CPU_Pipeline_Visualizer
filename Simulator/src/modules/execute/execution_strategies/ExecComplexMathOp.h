#pragma once

#include "../../../interfaces/IExecutionStrategy.cpp"

class ExecComplexMathOp: public IExecutionStrategy
{
private:
    std::string log(LoggablePackage toLog) override;
    std::string logComplex(Instruction instr, std::vector<word> highResult, std::vector<word> lowResult);
    bool handleNormalComplexOp(Instruction instr, word actualParam1, word actualParam2);
    bool handleZRegComplexOp(Instruction instr, std::vector<word> actualParam1, std::vector<word> actualParam2);
    
public:
    ExecComplexMathOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS,
        std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<Instruction>, SynchronizedDataPackage<address>>> commPipeWithDE,
        IClockBoundModule* refToEX,
        std::shared_ptr<CPURegisters> registers);
    void executeInstruction(SynchronizedDataPackage<Instruction> instrPackage) override;
};