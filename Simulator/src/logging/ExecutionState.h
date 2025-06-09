#pragma once

#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <map>
#include <sstream>
#include <memory>

#include "Config.h"
#include "Memory.h"
#include "Instruction.h"
#include "KWayCacheSet.h"
#include "SynchronizedDataPackage.h"
#include "MemoryAccessRequest.h"

struct ExecutionState
{
  static inline std::unordered_map<address, std::string> EXCEPTION_HANDLER_NAMES =
  {
    {DIV_BY_ZERO_HANDL, "\"Division by 0\""},
    {INVALID_DECODE_HANDL, "\"Invalid decode\""},
    {MISALIGNED_ACCESS_HANDL, "\"Misaligned memory access\""},
    {STACK_OVERFLOW_HANDL, "\"Stack overflow\""},
    {MISALIGNED_IP_HANDL, "\"Misaligned IP\""}
  };

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

  std::deque<word> stack = {};

  std::unordered_map<address, word> memoryChanges = {};
  clock_time memoryUnchangedSinceCycle = 1;

  struct {
    std::deque<SynchronizedDataPackage<address>> ICtoLS = {};
    std::deque<SynchronizedDataPackage<fetch_window>> LStoIC = {};
    std::deque<SynchronizedDataPackage<fetch_window>> ICtoDE = {};
    std::deque<SynchronizedDataPackage<address>> DEtoIC = {};
    std::deque<SynchronizedDataPackage<Instruction>> DEtoEX = {};
    std::deque<SynchronizedDataPackage<address>> EXtoDE = {};
    std::deque<SynchronizedDataPackage<MemoryAccessRequest>> EXtoLS = {};
    std::deque<SynchronizedDataPackage<std::vector<word>>> LStoEX = {};
  } pipes;

  struct {
    std::string state = "Idle";
    bool physicalMemoryAccessHappened = false;

    struct {
      unsigned int size = LS_CACHE_WORDS_SIZE * WORD_BYTES / sizeof(word);
      KWayCacheSet<word> storage[LS_CACHE_WORDS_SIZE * WORD_BYTES / sizeof(word)];
    } cache;
    std::string extra = "";
  } LS;

  struct {
    std::string state = "Idle";
    address internalIP = 0xFFF0;
    struct {
      unsigned int size = IC_CACHE_WORDS_SIZE * WORD_BYTES / sizeof(fetch_window);
      CacheLine<fetch_window> storage[IC_CACHE_WORDS_SIZE * WORD_BYTES / sizeof(fetch_window)];
    } cache;
    std::string extra = "";
  } IC;

  struct {
    std::string state = "Idle";
    struct {
      fetch_window storedFWs[DE_WORK_MEMORY_FW_SIZE] = {0};
      address cacheStartAddr = 0;
      byte storedWordsCount = 0;
    } fwTempStorage;
    std::string lastDecodedInstr = "None";
    std::string extra = "";
  } DE;

  struct {
    std::string state = "Idle";
    std::string activeException = "";
    std::string extra = "";
  } EX;

  static inline std::string hexW(const word&        value,
                                 const std::string& prefix = "0x",
                                 const byte&        padToLen = 4,
                                 const bool&        quotes = true);
  static inline std::string hexFW(const fetch_window&             value,
                                  const std::string&              divider = "_",
                                  const std::vector<std::string>& margins = {"[ ", " ]"},
                                  const bool&                    quotes = true,
                                  const byte&                     padToLen = 4,
                                  const std::string&              prefix = "");
  
  std::string toJSON();
};
