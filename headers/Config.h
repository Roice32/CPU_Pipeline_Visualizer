#pragma once

#include <cstdint>

#define ADDRESS_WIDTH 16
#define MEMORY_SIZE 65535
#define REGISTER_COUNT 8
#define FETCH_WINDOW_BYTES 8

#define byte uint8_t
#define register_16b uint16_t
#define address uint16_t
#define fetch_window uint64_t