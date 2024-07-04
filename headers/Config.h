#pragma once

#include <cstdint>

#define ADDRESS_WIDTH 16
#define MEMORY_SIZE 65535
#define REGISTER_COUNT 8
#define FETCH_WINDOW_BYTES 8

#define byte uint8_t
#define register_16b uint16_t
#define address uint16_t
#define word uint16_t
#define fetch_window uint64_t

enum OpCodes
{
    ADD = 0x1,
    SUB, 
    MOV,
    MUL,
    DIV,
    CMP,
    JMP,
    JE = 0x9,
    JL,
    JG,
    JZ,
    CALL,
    RET,
    END_SIM,
    PUSH,
    POP
};

enum TypeCodes
{
    NULL = 0x0,
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
    ADDR_R0 = 0x11,
    ADDR_R1,
    ADDR_R2,
    ADDR_R3,
    ADDR_R4,
    ADDR_R5,
    ADDR_R6,
    ADDR_R7,
};