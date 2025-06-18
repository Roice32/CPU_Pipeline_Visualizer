#include <sstream>
#include <iomanip>

#include "ExecutionState.h"

inline std::string ExecutionState::hexW(const word &value,
                                        const std::string &prefix,
                                        const byte &padToLen,
                                        const bool& quotes)
{
  static std::stringstream ss = std::stringstream();
  static std::string result;

  ss.str(""); // Clear the stringstream
  ss << std::setfill('0') << std::setw(padToLen) << std::hex << value;

  result = prefix + ss.str();
  if (quotes)
  {
    return "\"" + result + "\"";
  }
  return result;
}

inline std::string ExecutionState::hexFW(const fetch_window &value,
                                         const std::string &divider,
                                         const std::vector<std::string> &margins,
                                         const bool &quotes,
                                         const byte &padToLen,
                                         const std::string &prefix)
{
  static std::stringstream ss = std::stringstream();

  ss.str(""); // Clear the stringstream
  if (quotes)
    ss << "\"";
  ss << margins[0];
  for (byte i = 0; i < FETCH_WINDOW_WORDS; ++i)
  {
    byte shiftAmount = (FETCH_WINDOW_WORDS - 1 - i) * WORD_BYTES * 8;
    ss << hexW((value >> shiftAmount), prefix, padToLen, false);
    if (i < FETCH_WINDOW_WORDS - 1)
    {
      ss << divider;
    }
  }
  ss << margins[1];
  if (quotes)
    ss << "\"";
  return ss.str();
}

std::string ExecutionState::toJSON()
{
  static const byte ADDR_NIBBLES = ADDRESS_WIDTH / 4;
  static const byte WORD_NIBBLES = WORD_BYTES * 2;
  static std::stringstream ss = std::stringstream();

  ss.str(""); // Clear the stringstream

  // Start of JSON object
  ss << "{";

  // Registers section
  ss << "\"r\":{";

  // Regular registers (R0-R7)
  ss << "\"R\":[";
  for (int i = 0; i < REGISTER_COUNT; ++i) {
    ss << hexW(registers.R[i]);
    if (i < REGISTER_COUNT - 1)
      ss << ",";
  }
  ss << "],";

  // Vector registers (Z0-Z3)
  ss << "\"Z\":[";
  for (int i = 0; i < Z_REGISTER_COUNT; ++i) {
    ss << "[";
    for (int j = 0; j < WORDS_PER_Z_REGISTER; ++j) {
        ss << hexW(registers.Z[i][j]);
        if (j < WORDS_PER_Z_REGISTER - 1)
          ss << ",";
    }
    ss << "]";
    if (i < Z_REGISTER_COUNT - 1)
      ss << ",";
  }
  ss << "],";

  // Special registers
  ss << "\"i\":" << hexW(registers.IP, "#") << ",";
  ss << "\"f\":" << hexW(registers.flags) << ",";
  ss << "\"b\":" << hexW(registers.stackBase, "#") << ",";
  ss << "\"p\":" << hexW(registers.stackPointer, "#") << ",";
  ss << "\"s\":\"" << registers.stackSize << "\"";

  ss << "},"; // End of registers section

  // Stack section
  ss << "\"k\":[";
  while (!stack.empty()) {
    ss << hexW(stack.front());
    stack.pop_front();
    if (!stack.empty())
      ss << ",";
  }
  ss << "],";

  // Memory changes section
  ss << "\"u\":" << memoryUnchangedSinceCycle << ",";

  // Pipes section
  ss << "\"P\":{";

  // ICtoLS
  ss << "\"IL\":[";
  for (size_t i = 0; i < pipes.ICtoLS.size(); ++i) {
    ss << "{";
    ss << "\"d\":" << hexW(pipes.ICtoLS[i].data, "#") << ",";
    ss << "\"s\":" << pipes.ICtoLS[i].sentAt << ",";
    ss << "\"a\":" << hexW(pipes.ICtoLS[i].associatedIP, "#") << ",";
    ss << "\"e\":" << (pipes.ICtoLS[i].exceptionTriggered ? "true" : "false");
    if (pipes.ICtoLS[i].exceptionTriggered) {
      ss << ",\"x\":" << EXCEPTION_HANDLER_NAMES[pipes.ICtoLS[i].handlerAddr] << ",";
      ss << "\"h\":" << hexW(pipes.ICtoLS[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.ICtoLS.size() - 1)
      ss << ",";
  }
  ss << "],";

  // LStoIC
  ss << "\"LI\":[";
  for (size_t i = 0; i < pipes.LStoIC.size(); ++i) {
    ss << "{";
    bool isCacheInvalidate = pipes.LStoIC[i].exceptionTriggered && pipes.LStoIC[i].data % 2 == 0;
    if (isCacheInvalidate) {
      ss << "\"d\": \"Invalidate\\ncache\\nline\","; 
    }
    else {
      ss << "\"d\":" << hexFW(pipes.LStoIC[i].data, "", {"", ""}) << ",";
    }
    ss << "\"s\":" << pipes.LStoIC[i].sentAt << ",";
    ss << "\"a\":" << hexW( isCacheInvalidate
                          ? pipes.LStoIC[i].associatedIP
                          : pipes.LStoIC[i].data, "#") << ",";
    bool isActualException = pipes.LStoIC[i].exceptionTriggered && pipes.LStoIC[i].data % 2 == 1;
    ss << "\"e\":" << (isActualException ? "true" : "false");
    if (isActualException) {
        ss << ",\"x\":" << EXCEPTION_HANDLER_NAMES[pipes.LStoIC[i].handlerAddr] << ",";
        ss << "\"h\":" << hexW(pipes.LStoIC[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.LStoIC.size() - 1)
      ss << ",";
  }
  ss << "],";

  // ICtoDE
  ss << "\"ID\":[";
  for (size_t i = 0; i < pipes.ICtoDE.size(); ++i) {
    ss << "{";
    ss << "\"d\":" << hexFW(pipes.ICtoDE[i].data, "", {"", ""}) << ",";
    ss << "\"s\":" << pipes.ICtoDE[i].sentAt << ",";
    ss << "\"a\":" << hexW(pipes.ICtoDE[i].associatedIP, "#") << ",";
    ss << "\"e\":" << (pipes.ICtoDE[i].exceptionTriggered ? "true" : "false");
    if (pipes.ICtoDE[i].exceptionTriggered) {
        ss << ",\"x\":" << EXCEPTION_HANDLER_NAMES[pipes.ICtoDE[i].handlerAddr] << ",";
        ss << "\"h\":" << hexW(pipes.ICtoDE[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.ICtoDE.size() - 1)
      ss << ",";
  }
  ss << "],";

  // DEtoIC
  ss << "\"DI\":[";
  for (size_t i = 0; i < pipes.DEtoIC.size(); ++i) {
    ss << "{";
    ss << "\"d\":" << hexW(pipes.DEtoIC[i].data, "#") << ",";
    ss << "\"s\":" << pipes.DEtoIC[i].sentAt << ",";
    ss << "\"a\":" << hexW(pipes.DEtoIC[i].associatedIP, "#") << ",";
    ss << "\"e\":" << (pipes.DEtoIC[i].exceptionTriggered ? "true" : "false");
    if (pipes.DEtoIC[i].exceptionTriggered) {
        ss << ",\"x\":" << EXCEPTION_HANDLER_NAMES[pipes.DEtoIC[i].handlerAddr] << ",";
        ss << "\"h\":" << hexW(pipes.DEtoIC[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.DEtoIC.size() - 1)
      ss << ",";
  }
  ss << "],";

  // DEtoEX
  ss << "\"DE\":[";
  for (size_t i = 0; i < pipes.DEtoEX.size(); ++i) {
    ss << "{";
    ss << "\"d\":\"";
      ss << "OpCode: " << hexW(pipes.DEtoEX[i].data.opCode, "", 2, false) << "\\n";
      ss << "Src1: " << hexW(pipes.DEtoEX[i].data.src1, "", 2, false) << "\\n";
      ss << "Src2: " << hexW(pipes.DEtoEX[i].data.src2, "", 2, false) << "\\n";
      ss << "Param1: " << hexW(pipes.DEtoEX[i].data.param1, "", 4, false) << "\\n";
      ss << "Param2: " << hexW(pipes.DEtoEX[i].data.param2, "", 4, false);
    ss << "\",";
    ss << "\"s\":" << pipes.DEtoEX[i].sentAt << ",";
    ss << "\"a\":" << hexW(pipes.DEtoEX[i].associatedIP, "#") << ",";
    ss << "\"e\":" << (pipes.DEtoEX[i].exceptionTriggered ? "true" : "false");
    if (pipes.DEtoEX[i].exceptionTriggered) {
        ss << ",\"x\":" << EXCEPTION_HANDLER_NAMES[pipes.DEtoEX[i].handlerAddr] << ",";
        ss << "\"h\":" << hexW(pipes.DEtoEX[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.DEtoEX.size() - 1)
      ss << ",";
  }
  ss << "],";

  // EXtoDE
  ss << "\"ED\":[";
  for (size_t i = 0; i < pipes.EXtoDE.size(); ++i) {
    ss << "{";
    ss << "\"d\":" << hexW(pipes.EXtoDE[i].data, "#") << ",";
    ss << "\"s\":" << pipes.EXtoDE[i].sentAt << ",";
    ss << "\"a\":" << hexW(pipes.EXtoDE[i].associatedIP, "#") << ",";
    ss << "\"e\":" << (pipes.EXtoDE[i].exceptionTriggered ? "true" : "false");
    if (pipes.EXtoDE[i].exceptionTriggered) {
        ss << ",\"x\":" << EXCEPTION_HANDLER_NAMES[pipes.EXtoDE[i].handlerAddr] << ",";
        ss << "\"h\":" << hexW(pipes.EXtoDE[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.EXtoDE.size() - 1)
      ss << ",";
  }
  ss << "],";

  // EXtoLS
  ss << "\"EL\":[";
  for (size_t i = 0; i < pipes.EXtoLS.size(); ++i) {
    ss << "{";
    ss << "\"d\":\"";
      ss << "Request Addr.:\\n" << hexW(pipes.EXtoLS[i].data.reqAddr, "#", 4, false);
      ss << "\\nWords Count: " << std::to_string(pipes.EXtoLS[i].data.wordsSizeOfReq) << "";
      ss << "\\nIs Read Op?\\n" << (pipes.EXtoLS[i].data.isStoreOperation ? "No" : "Yes");
      if (pipes.EXtoLS[i].data.isStoreOperation) {
        ss << "\\nWords:";
        for (size_t j = 0; j < pipes.EXtoLS[i].data.reqData.size(); ++j) {
            ss << "\\n" << hexW(pipes.EXtoLS[i].data.reqData[j], "0x", 4, false);
        }
      }
    ss << "\",";
    ss << "\"s\":" << pipes.EXtoLS[i].sentAt << ",";
    ss << "\"a\":" << hexW(pipes.EXtoLS[i].associatedIP, "#") << ",";
    ss << "\"e\":" << (pipes.EXtoLS[i].exceptionTriggered ? "true" : "false");
    if (pipes.EXtoLS[i].exceptionTriggered) {
        ss << ",\"x\":" << EXCEPTION_HANDLER_NAMES[pipes.EXtoLS[i].handlerAddr] << ",";
        ss << "\"h\":" << hexW(pipes.EXtoLS[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.EXtoLS.size() - 1)
      ss << ",";
  }
  ss << "],";

  // LStoEX
  ss << "\"LE\":[";
  for (size_t i = 0; i < pipes.LStoEX.size(); ++i) {
    ss << "{";
    ss << "\"d\":\"";
    if (pipes.LStoEX[i].data.empty()) {
        ss << "None";
    }
    else {
      for (size_t j = 0; j < pipes.LStoEX[i].data.size(); ++j) {
          ss << hexW(pipes.LStoEX[i].data[j], "0x", 4, false);
          if (j < pipes.LStoEX[i].data.size() - 1)
            ss << "\\n";
      }
    }
    ss << "\",";
    ss << "\"s\":" << pipes.LStoEX[i].sentAt << ",";
    ss << "\"a\":" << hexW(pipes.LStoEX[i].associatedIP, "#") << ",";
    ss << "\"e\":" << (pipes.LStoEX[i].exceptionTriggered ? "true" : "false");
    if (pipes.LStoEX[i].exceptionTriggered) {
        ss << ",\"x\":" << EXCEPTION_HANDLER_NAMES[pipes.LStoEX[i].handlerAddr] << ",";
        ss << "\"h\":" << hexW(pipes.LStoEX[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.LStoEX.size() - 1)
      ss << ",";
  }
  ss << "]";

  ss << "},"; // End of pipes section

  // LS stage
  ss << "\"L\":{";
  ss << "\"s\":\"" << LS.state << "\",";

  // LS cache
  ss << "\"c\":{";
  ss << "\"s\":" << LS.cache.size << ",";
  ss << "\"e\":[";
  for (unsigned int i = 0; i < LS.cache.size; ++i) {
    ss << "["; // Start of a cache set
    for (size_t j = 0; j < LS.cache.storage[i].storedLines.size(); ++j) {
        ss << "{";
        ss << "\"d\":" << hexW(LS.cache.storage[i].storedLines[j].data) << ",";
        ss << "\"t\":" << hexW(LS.cache.storage[i].storedLines[j].tag, "#") << ",";
        ss << "\"l\":" << LS.cache.storage[i].storedLines[j].lastHitTime << ",";
        ss << "\"v\":" << (LS.cache.storage[i].storedLines[j].valid ? "true" : "false") << ",";
        ss << "\"m\":" << (LS.cache.storage[i].storedLines[j].modified ? "true" : "false");
        ss << "}";
        if (j < LS.cache.storage[i].storedLines.size() - 1)
          ss << ",";
    }
    ss << "]"; // End of a cache set
    if (i < LS.cache.size - 1)
      ss << ",";
  }
  ss << "]"; // End of LS cache storage
  ss << "}"; // End of LS cache

  ss << ",\"x\":\"" << LS.extra << "\""; // Extra information for LS stage
  ss << "},"; // End of LS stage

  // IC stage
  ss << "\"I\":{";
  ss << "\"s\":\"" << IC.state << "\",";
  ss << "\"i\":" << hexW(IC.internalIP, "#") << ",";

  // IC cache
  ss << "\"c\":{";
  ss << "\"e\":[";
  for (unsigned int i = 0; i < IC.cache.size; ++i) {
    ss << "{";
    ss << "\"d\":" << hexFW(IC.cache.storage[i].data, "", {"", ""}) << ",";
    ss << "\"t\": \"" << convDecToHex(IC.cache.storage[i].tag) << "\",";
    ss << "\"v\":" << (IC.cache.storage[i].valid ? "true" : "false");
    ss << "}";
    if (i < IC.cache.size - 1)
      ss << ",";
  }
  ss << "]"; // End of IC cache storage
  ss << "}"; // End of IC cache

  ss << ",\"x\":\"" << IC.extra << "\""; // Extra information for IC stage
  ss << "},"; // End of IC stage

  // DE stage
  ss << "\"D\":{";
  ss << "\"s\":\"" << DE.state << "\",";

  // DE fw temp storage
  ss << "\"t\":{";
  ss << "\"e\":\"[";
  for (int i = 0; i < DE_WORK_MEMORY_FW_SIZE; ++i) {
    ss << hexFW(DE.fwTempStorage.storedFWs[i], "_", {"", ""}, false);
    if (i < DE_WORK_MEMORY_FW_SIZE - 1)
      ss << "_";
  }
  ss << "]\",";
  ss << "\"a\":" << hexW(DE.fwTempStorage.cacheStartAddr, "#") << ",";
  ss << "\"c\":" << std::to_string(DE.fwTempStorage.storedWordsCount);
  ss << "}"; // End of DE fw temp storage

  ss << ",\"l\":\"" << DE.lastDecodedInstr << "\"";
  ss << ",\"x\":\"" << DE.extra << "\""; // Extra information for DE stage
  ss << "},"; // End of DE stage

  // EX stage
  ss << "\"E\":{";
  ss << "\"s\":\"" << EX.state << "\"";
  ss << ",\"u\":\"" << EX.substate << "\"";
  ss << ",\"e\":\"" << EX.activeException << "\"";
  ss << ",\"x\":\"" << EX.extra << "\""; // Extra information for EX stage
  ss << "}"; // End of EX stage
  ss << "}"; // End of JSON object

  return ss.str();
}