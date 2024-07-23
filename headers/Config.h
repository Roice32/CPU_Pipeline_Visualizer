#pragma once

#include <cstdint>

#define ADDRESS_WIDTH 16
#define MEMORY_SIZE 0xffff
#define REGISTER_COUNT 8
#define WORD_BYTES 2
#define FETCH_WINDOW_BYTES 8
#define DECODER_CACHE_FW_SIZE 2
#define CLOCK_PERIOD_MILLIS 1
#define DUMMY_ADDRESS address (-1)
#define SAVE_STATE_ADDR 0x0010

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
    EXCP_EXIT
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
    ADDR_R0 = 0x18,
    ADDR_R1,
    ADDR_R2,
    ADDR_R3,
    ADDR_R4,
    ADDR_R5,
    ADDR_R6,
    ADDR_R7,
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
    MUTUALLY_EXCLUSIVE_SRCS = 0x0008,
    PUSH_OVERFLOW = 0x0000,
    POP_OVERFLOW = 0xffff,
    MISALIGNED_IP = 0x0000
};