#pragma once

#include <unordered_map>

#include "Config.h"

class Instruction
{
private:
  static inline std::unordered_map<OpCode, std::string> opNames;
  static inline std::unordered_map<TypeCode, std::string> typeNames;
  static void initializeNames()
  {
    opNames = {};
    opNames.insert({ADD, "add"});
    opNames.insert({SUB, "sub"});
    opNames.insert({MOV, "mov"});
    opNames.insert({MUL, "mul"});
    opNames.insert({DIV, "div"});
    opNames.insert({CMP, "cmp"});
    opNames.insert({JMP, "jmp"});
    opNames.insert({JE, "je"});
    opNames.insert({JL, "jl"});
    opNames.insert({JG, "jg"});
    opNames.insert({JZ, "jz"});
    opNames.insert({CALL, "call"});
    opNames.insert({RET, "ret"});
    opNames.insert({END_SIM, "end_sim"});
    opNames.insert({PUSH, "push"});
    opNames.insert({POP, "pop"});
    opNames.insert({EXCP_EXIT, "excp_exit"});
    opNames.insert({GATHER, "gather"});
    opNames.insert({SCATTER, "scatter"});

    typeNames = {};
    typeNames.insert({NULL_VAL, ""});
    typeNames.insert({SP_REG, "sp"});
    typeNames.insert({ST_BASE, "stack_base"});
    typeNames.insert({ST_SIZE, "stack_size"});
    typeNames.insert({R0, "r0"});
    typeNames.insert({R1, "r1"});
    typeNames.insert({R2, "r2"});
    typeNames.insert({R3, "r3"});
    typeNames.insert({R4, "r4"});
    typeNames.insert({R5, "r5"});
    typeNames.insert({R6, "r6"});
    typeNames.insert({R7, "r7"});
    typeNames.insert({ADDR_R0, "[r0]"});
    typeNames.insert({ADDR_R1, "[r1]"});
    typeNames.insert({ADDR_R2, "[r2]"});
    typeNames.insert({ADDR_R3, "[r3]"});
    typeNames.insert({ADDR_R4, "[r4]"});
    typeNames.insert({ADDR_R5, "[r5]"});
    typeNames.insert({ADDR_R6, "[r6]"});
    typeNames.insert({ADDR_R7, "[r7]"});
    typeNames.insert({Z0, "z0"});
    typeNames.insert({Z1, "z1"});
    typeNames.insert({Z2, "z2"});
    typeNames.insert({Z3, "z3"});
  }

  static bool mustDisplayParamValue(byte src)
  {
    return src == IMM || src == ADDR;
  }

  static std::string plainArgToString(byte src, word param, bool spaced = true)
  {
    std::string result = "";
    if (spaced)
      result += " ";
    if (mustDisplayParamValue(src))
    {
      if (src == IMM)
        result += std::to_string(param);
      else
        result += "[" + convDecToHex(param) + "]";
    }
    else
    {
      auto argNameFound = typeNames.find((TypeCode) src);
      if (argNameFound == typeNames.end())
        result += "???";
      else
        result += typeNames.at((TypeCode) src);
    }
    return result;
  }

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

  std::string toString() const
  {
    if (opNames.empty())
      initializeNames();

    std::string result = opNames.at((OpCode) opCode);
    result += plainArgToString(src1, param1);
    if (src1 * src2 != NULL_VAL)
      result += ", ";
    result += plainArgToString(src2, param2);
    return result;
  }
};