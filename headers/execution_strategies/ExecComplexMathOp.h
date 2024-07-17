#pragma once

#include "IExecutionStrategy.cpp"

class ExecComplexMathOp: public IExecutionStrategy
{
private:
    void log(Instruction instr, word r0Result = 0, word r1Result = 0, bool newLine = true) override;
    
public:
    ExecComplexMathOp(std::shared_ptr<InterThreadCommPipe<SynchronizedDataPackage<MemoryAccessRequest>, SynchronizedDataPackage<std::vector<word>>>> commPipeWithLS, std::shared_ptr<CPURegisters> registers);
    void executeInstruction(Instruction instr) override;
};