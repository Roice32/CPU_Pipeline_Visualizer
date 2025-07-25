#pragma once

#include <cstdint>
#include <string>
#include <iostream>

using byte = uint8_t;
using register_16b = uint16_t;
using address = uint16_t;
using word = uint16_t;
using fetch_window = uint64_t;
using clock_time = uint64_t;

#define WORD_BYTES 2

#define REGISTER_COUNT 8
#define Z_REGISTER_COUNT 4
#define WORDS_PER_Z_REGISTER 4

#define FETCH_WINDOW_BYTES 8
#define FETCH_WINDOW_WORDS 4
#define DE_WORK_MEMORY_FW_SIZE 2

#define ADDRESS_WIDTH 16
#define SAVE_STATE_ADDR 0x0010
#define SIM_START_ADDR 0xfff0
#define DUMMY_ADDRESS address (-1)

extern bool GARBAGE_MEMORY;
extern bool SINGLE_STATE_MODE;
extern bool IGNORE_UNINITIALIZED_MEM;

extern byte CLOCK_PERIOD_MILLIS;
extern clock_time CYCLES_LIMIT;

extern byte IC_CYCLES_PER_OP;
extern byte IC_CYCLES_PER_OP_WITH_CACHE_HIT;
extern byte LS_CYCLES_PER_OP;
extern byte LS_CYCLES_PER_OP_WITH_CACHE_HIT;
extern byte DE_CYCLES_PER_OP;
extern byte EX_CYCLES_PER_OP;

extern word IC_CACHE_WORDS_SIZE;
extern word LS_CACHE_WORDS_SIZE;
extern byte LS_CACHE_SET_ENTRIES_COUNT;

enum OpCode
{
  UNINITIALIZED_MEM,
  ADD = 0x1,
  SUB, 
  MOV,
  MUL,
  DIV,
  CMP,
  JMP,
  UNDEFINED,
  JE,
  JL,
  JG,
  JZ,
  CALL,
  RET,
  END_SIM,
  PUSH,
  POP,
  EXCP_EXIT,
  GATHER = 0x16,
  SCATTER
};

enum TypeCode
{
  NULL_VAL = 0x0,
  IMM,
  ADDR,
  SP_REG,
  ST_BASE,
  ST_SIZE,
  R0 = 0x8,
  R1,
  R2,
  R3,
  R4,
  R5,
  R6,
  R7,
  Z0 = 0x10,
  Z1,
  Z2,
  Z3,
  ADDR_R0 = 0x18,
  ADDR_R1,
  ADDR_R2,
  ADDR_R3,
  ADDR_R4,
  ADDR_R5,
  ADDR_R6,
  ADDR_R7
};

enum Flags
{
  ZERO = 0x8000,
  EQUAL = 0x4000,
  GREATER = 0x2000,
  EXCEPTION = 0x0800,
};

enum ExceptionHandlerAddress
{
  DIV_BY_ZERO_HANDL = 0x0000,
  INVALID_DECODE_HANDL = 0x0002,
  MISALIGNED_ACCESS_HANDL = 0x0004,
  STACK_OVERFLOW_HANDL = 0x0006,
  MISALIGNED_IP_HANDL = 0x0008
};

enum Exception
{
  DIV_BY_ZERO = 0x0000,
  UNKNOWN_OP_CODE = 0x0001,
  NULL_SRC = 0x0002,
  NON_NULL_SRC = 0x0004,
  INCOMPATIBLE_PARAMS = 0x0008,
  PUSH_OVERFLOW = 0x0000,
  POP_OVERFLOW = 0xffff,
  MISALIGNED_IP = 0x0000
};

inline bool isReg(byte src)
{
  return src >= R0 && src <= R7;
}

inline bool isAddrReg(byte src)
{
  return src >= ADDR_R0 && src <= ADDR_R7;
}

inline bool isZReg(byte src)
{
  return src >= Z0 && src <= Z3;
}

inline std::string convDecToHex(word source)
{
  std::string result = "xxxx";
  byte bytesGroup;
  for (int ind = WORD_BYTES * 2 - 1; ind >= 0; --ind)
  {
    bytesGroup = source & 0xf;
    result.at(ind) = (bytesGroup > 9) ? ('a' + bytesGroup - 10) : ('0' + bytesGroup);
    source >>= 4;
  }
  return result;
}

inline std::string fwToStr(fetch_window source)
{
  std::string result = "[";
  for (byte ind = 0; ind < FETCH_WINDOW_WORDS; ++ind)
  {
    byte shiftAmount = (FETCH_WINDOW_BYTES - ind - 1) * WORD_BYTES * 8;
    result += convDecToHex((source >> shiftAmount) & 0xffff);
    if (ind != FETCH_WINDOW_WORDS - 1)
      result += " ";
  }
  result += "]";
  return result;
}
