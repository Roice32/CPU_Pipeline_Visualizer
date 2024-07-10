#pragma once

#include <cstdint>

#define ADDRESS_WIDTH 16
#define MEMORY_SIZE 65535
#define REGISTER_COUNT 8
#define FETCH_WINDOW_BYTES 8

#define INPUT_FILE_PATH "../program_files/allOpsTest.hex"

using byte = uint8_t;
using register_16b = uint16_t;
using address = uint16_t;
using word = uint16_t;
using fetch_window = uint64_t;

enum OpCode
{
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
    POP
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
    GREATER = 0x2000
};