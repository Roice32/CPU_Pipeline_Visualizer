#include "Execute.h"
#include "Config.h"

Execute::Execute(LoadStore* lsModule, register_16b* ip, InstructionCache* icModule): IMemoryAccesser(lsModule)
{
    IP = ip;
    ICModule = icModule;
}

word Execute::requestDataAt(word addr)
{
    word result = 0;
    result = LSModule->bufferedLoadFrom(addr);
    result <<= 8;
    result |= LSModule->bufferedLoadFrom(addr + 8);
    return result;
}

void Execute::storeDataAt(word addr, word data)
{
    LSModule->storeAt(addr, data >> 8);
    LSModule->storeAt(addr + 8, (data << 8) >> 8);
}

void Execute::executeInstruction(Instruction instr)
{
    if (instr.opCode == END_SIM)
        *IP = 0xffff;
    // Made to work only for jumps to labels fttb
    if (instr.opCode == JMP)
        *IP = instr.param1;
    ICModule->requestFetchWindow();
}