#pragma once

#include <cstdint>
#include <string>

#define CLOCK_PERIOD_MILLIS 1

#define IC_CYCLES_PER_OP 3
#define IC_CYCLES_PER_OP_WITH_CACHE_HIT 2

#define ADDRESS_WIDTH 16
#define SAVE_STATE_ADDR 0x0010
#define DUMMY_ADDRESS address (-1)
#define SIM_START_ADDR 0xfff0

#define REGISTER_COUNT 8
#define Z_REGISTER_COUNT 4
#define WORDS_PER_Z_REGISTER 4

#define WORD_BYTES 2
#define FETCH_WINDOW_BYTES 8
#define FETCH_WINDOW_WORDS 4
#define DE_WORK_MEMORY_FW_SIZE 2

#define IC_CACHE_WORDS_SIZE 64
#define LS_CACHE_WORDS_SIZE 64
#define LS_CACHE_SET_SIZE 2
#define BP_CACHE_ENTRIES_SIZE 16

using byte = uint8_t;
using register_16b = uint16_t;
using address = uint16_t;
using word = uint16_t;
using fetch_window = uint64_t;
using clock_time = uint64_t;

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
  for (byte ind = 0; ind < WORD_BYTES * 2; ++ind)
  {
    bytesGroup = source & 0xf;
    result.at(WORD_BYTES * 2 - ind - 1) = (bytesGroup > 9) ? ('a' + bytesGroup - 10) : ('0' + bytesGroup);
    source >>= 4;
  }
  return result;
}

inline std::string fwToStr(fetch_window source)
{
  std::string result = "[";
  for (byte ind = 0; ind < FETCH_WINDOW_WORDS; ++ind)
  {
    result += convDecToHex(source & 0xffff);
    if (ind != FETCH_WINDOW_WORDS - 1)
      result += " ";
    source >>= WORD_BYTES * 8;
  }
  result += "]";
  return result;
}