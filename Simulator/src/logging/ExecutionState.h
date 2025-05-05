#pragma once

#include "string"
#include "vector"
#include "stack"
#include "unordered_map"
#include "sstream"
#include "memory"

#include "../Config.h"
#include "../Memory/Memory.h"
#include "../Instruction.h"
#include "../cache/KWayCacheSet.h"
#include "../synchronization/SynchronizedDataPackage.h"
#include "../synchronization/MemoryAccessRequest.h"

struct ExecutionState
{
  clock_time cycle = 1;

  struct {
    word R[REGISTER_COUNT] = {0};
    word Z[Z_REGISTER_COUNT][WORDS_PER_Z_REGISTER] = {0};
    word IP = 0xFFF0;
    word flags = 0;
    word stackBase = 0xEFEE;
    word stackSize = 4096;
    word stackPointer = 4096;
  } registers;

  std::stack<word> stack = {};

  std::unordered_map<address, word> memory = {};

  struct {
    std::vector<SynchronizedDataPackage<address>> ICtoLS = {};
    std::vector<SynchronizedDataPackage<fetch_window>> LStoIC = {};
    std::vector<SynchronizedDataPackage<fetch_window>> ICtoDE = {};
    std::vector<SynchronizedDataPackage<address>> DEtoIC = {};
    std::vector<SynchronizedDataPackage<Instruction>> DEtoEX = {};
    std::vector<SynchronizedDataPackage<address>> EXtoDE = {};
    std::vector<SynchronizedDataPackage<MemoryAccessRequest>> EXtoLS = {};
    std::vector<SynchronizedDataPackage<std::vector<word>>> LStoEX = {};
  } pipes;

  struct {
    std::string state = "Idle";
    bool physicalMemoryAccessHappened = false;

    struct {
      unsigned int size = LS_CACHE_WORDS_SIZE * WORD_BYTES / sizeof(word);
      KWayCacheSet<word> storage[LS_CACHE_WORDS_SIZE * WORD_BYTES / sizeof(word)];
      byte currReqIndex = 0;
      address currReqTag = 0;
      byte foundIndex = 0;
    } cache;
  } LS;

  struct {
    std::string state = "Idle";
    address internalIP = 0xFFF0;
    struct {
      unsigned int size = IC_CACHE_WORDS_SIZE * WORD_BYTES / sizeof(fetch_window);
      CacheLine<fetch_window> storage[IC_CACHE_WORDS_SIZE * WORD_BYTES / sizeof(fetch_window)];
      byte currReqIndex = 0;
      byte currReqTag = 0;
    } cache;
  } IC;

  struct {
    std::string state = "Idle";
    struct {
      fetch_window storedFWs[DE_WORK_MEMORY_FW_SIZE] = {0};
      address cacheStartAddr = 0;
      byte storedWordsCount = 0;
    } fwTempStorage;
  } DE;

  struct {
    std::string state = "Idle";
  } EX;

  std::string toJSON() const;
};
