#pragma once

#include "Config.h"

class Instruction
{
public:
  byte opCode;
  byte src1, src2;
  word param1, param2;

  Instruction() {};
  Instruction(byte opCode): opCode(opCode) {};
  Instruction(byte opCode, byte src1): Instruction(opCode)
    { this->src1 = src1; }
  Instruction(byte opCode, byte src1, byte src2): Instruction(opCode, src1)
    { this->src2 = src2; }
};