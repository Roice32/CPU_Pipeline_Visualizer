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
  
  // Basic execution state properties
  ss << "\"cycle\":" << cycle << ",";
  
  // Registers section
  ss << "\"registers\":{";
  
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
  ss << "\"IP\":" << hexW(registers.IP, "#") << ",";
  ss << "\"flags\":" << hexW(registers.flags) << ",";
  ss << "\"stackBase\":" << hexW(registers.stackBase, "#") << ",";
  ss << "\"stackPointer\":" << hexW(registers.stackPointer, "#") << ",";
  ss << "\"stackSize\":\"" << registers.stackSize << "\"";
  
  ss << "},"; // End of registers section

  // Stack section
  ss << "\"stack\":[";
  while (!stack.empty()) {
    ss << hexW(stack.front());
    stack.pop_front();
    if (!stack.empty())
      ss << ",";
  }
  ss << "],";
  
  // Memory changes section
  ss << "\"memoryUnchangedSinceCycle\":" << memoryUnchangedSinceCycle << ",";

  // Pipes section
  ss << "\"pipes\":{";
  
  // ICtoLS
  ss << "\"ICtoLS\":[";
  for (size_t i = 0; i < pipes.ICtoLS.size(); ++i) {
    ss << "{";
    ss << "\"data\":" << hexW(pipes.ICtoLS[i].data, "#") << ",";
    ss << "\"sentAt\":" << pipes.ICtoLS[i].sentAt << ",";
    ss << "\"associatedIP\":" << hexW(pipes.ICtoLS[i].associatedIP, "#") << ",";
    ss << "\"exceptionTriggered\":" << (pipes.ICtoLS[i].exceptionTriggered ? "true" : "false");
    if (pipes.ICtoLS[i].exceptionTriggered) {
      ss << ",\"excpData\":" << EXCEPTION_HANDLER_NAMES[pipes.ICtoLS[i].handlerAddr] << ",";
      ss << "\"handlerAddr\":" << hexW(pipes.ICtoLS[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.ICtoLS.size() - 1)
      ss << ",";
  }
  ss << "],";
  
  // LStoIC
  ss << "\"LStoIC\":[";
  for (size_t i = 0; i < pipes.LStoIC.size(); ++i) {
    ss << "{";
    ss << "\"data\":" << hexFW(pipes.LStoIC[i].data, "", {"", ""}) << ",";
    ss << "\"sentAt\":" << pipes.LStoIC[i].sentAt << ",";
    ss << "\"associatedIP\":" << hexW(pipes.LStoIC[i].associatedIP, "#") << ",";
    ss << "\"exceptionTriggered\":" << (pipes.LStoIC[i].exceptionTriggered ? "true" : "false");
    if (pipes.LStoIC[i].exceptionTriggered) {
        ss << ",\"excpData\":" << EXCEPTION_HANDLER_NAMES[pipes.LStoIC[i].handlerAddr] << ",";
        ss << "\"handlerAddr\":" << hexW(pipes.LStoIC[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.LStoIC.size() - 1)
      ss << ",";
  }
  ss << "],";
  
  // ICtoDE
  ss << "\"ICtoDE\":[";
  for (size_t i = 0; i < pipes.ICtoDE.size(); ++i) {
    ss << "{";
    ss << "\"data\":" << hexFW(pipes.ICtoDE[i].data, "", {"", ""}) << ",";
    ss << "\"sentAt\":" << pipes.ICtoDE[i].sentAt << ",";
    ss << "\"associatedIP\":" << hexW(pipes.ICtoDE[i].associatedIP, "#") << ",";
    ss << "\"exceptionTriggered\":" << (pipes.ICtoDE[i].exceptionTriggered ? "true" : "false");
    if (pipes.ICtoDE[i].exceptionTriggered) {
        ss << ",\"excpData\":" << EXCEPTION_HANDLER_NAMES[pipes.ICtoDE[i].handlerAddr] << ",";
        ss << "\"handlerAddr\":" << hexW(pipes.ICtoDE[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.ICtoDE.size() - 1)
      ss << ",";
  }
  ss << "],";
  
  // DEtoIC
  ss << "\"DEtoIC\":[";
  for (size_t i = 0; i < pipes.DEtoIC.size(); ++i) {
    ss << "{";
    ss << "\"data\":" << hexW(pipes.DEtoIC[i].data, "#") << ",";
    ss << "\"sentAt\":" << pipes.DEtoIC[i].sentAt << ",";
    ss << "\"associatedIP\":" << hexW(pipes.DEtoIC[i].associatedIP, "#") << ",";
    ss << "\"exceptionTriggered\":" << (pipes.DEtoIC[i].exceptionTriggered ? "true" : "false");
    if (pipes.DEtoIC[i].exceptionTriggered) {
        ss << ",\"excpData\":" << EXCEPTION_HANDLER_NAMES[pipes.DEtoIC[i].handlerAddr] << ",";
        ss << "\"handlerAddr\":" << hexW(pipes.DEtoIC[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.DEtoIC.size() - 1)
      ss << ",";
  }
  ss << "],";
  
  // DEtoEX
  ss << "\"DEtoEX\":[";
  for (size_t i = 0; i < pipes.DEtoEX.size(); ++i) {
    ss << "{";
    ss << "\"data\":\"";
    ss << "OpCode: " << hexW(pipes.DEtoEX[i].data.opCode, "", 2, false) << "\\n";
    ss << "Src1: " << hexW(pipes.DEtoEX[i].data.src1, "", 2, false) << "\\n";
    ss << "Src2: " << hexW(pipes.DEtoEX[i].data.src2, "", 2, false) << "\\n";
    ss << "Param1: " << hexW(pipes.DEtoEX[i].data.param1, "", 4, false) << "\\n";
    ss << "Param2: " << hexW(pipes.DEtoEX[i].data.param2, "", 4, false);
    ss << "\",";
    ss << "\"sentAt\":" << pipes.DEtoEX[i].sentAt << ",";
    ss << "\"associatedIP\":" << hexW(pipes.DEtoEX[i].associatedIP, "#") << ",";
    ss << "\"exceptionTriggered\":" << (pipes.DEtoEX[i].exceptionTriggered ? "true" : "false");
    if (pipes.DEtoEX[i].exceptionTriggered) {
        ss << ",\"excpData\":" << EXCEPTION_HANDLER_NAMES[pipes.DEtoEX[i].handlerAddr] << ",";
        ss << "\"handlerAddr\":" << hexW(pipes.DEtoEX[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.DEtoEX.size() - 1)
      ss << ",";
  }
  ss << "],";
  
  // EXtoDE
  ss << "\"EXtoDE\":[";
  for (size_t i = 0; i < pipes.EXtoDE.size(); ++i) {
    ss << "{";
    ss << "\"data\":" << hexW(pipes.EXtoDE[i].data, "#") << ",";
    ss << "\"sentAt\":" << pipes.EXtoDE[i].sentAt << ",";
    ss << "\"associatedIP\":" << hexW(pipes.EXtoDE[i].associatedIP, "#") << ",";
    ss << "\"exceptionTriggered\":" << (pipes.EXtoDE[i].exceptionTriggered ? "true" : "false");
    if (pipes.EXtoDE[i].exceptionTriggered) {
        ss << ",\"excpData\":" << EXCEPTION_HANDLER_NAMES[pipes.EXtoDE[i].handlerAddr] << ",";
        ss << "\"handlerAddr\":" << hexW(pipes.EXtoDE[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.EXtoDE.size() - 1)
      ss << ",";
  }
  ss << "],";
  
  // EXtoLS
  ss << "\"EXtoLS\":[";
  for (size_t i = 0; i < pipes.EXtoLS.size(); ++i) {
    ss << "{";
    ss << "\"data\":{";
    ss << "\"reqAddr\":" << hexW(pipes.EXtoLS[i].data.reqAddr, "#") << ",";
    ss << "\"wordsSizeOfReq\":" << std::to_string(pipes.EXtoLS[i].data.wordsSizeOfReq) << ",";
    ss << "\"isStoreOperation\":" << (pipes.EXtoLS[i].data.isStoreOperation ? "true" : "false") << ",";
    ss << "\"reqData\":[";
    for (size_t j = 0; j < pipes.EXtoLS[i].data.reqData.size(); ++j) {
        ss << hexW(pipes.EXtoLS[i].data.reqData[j]);
        if (j < pipes.EXtoLS[i].data.reqData.size() - 1)
          ss << ",";
    }
    ss << "]";
    ss << "},";
    ss << "\"sentAt\":" << pipes.EXtoLS[i].sentAt << ",";
    ss << "\"associatedIP\":" << hexW(pipes.EXtoLS[i].associatedIP, "#") << ",";
    ss << "\"exceptionTriggered\":" << (pipes.EXtoLS[i].exceptionTriggered ? "true" : "false");
    if (pipes.EXtoLS[i].exceptionTriggered) {
        ss << ",\"excpData\":" << EXCEPTION_HANDLER_NAMES[pipes.EXtoLS[i].handlerAddr] << ",";
        ss << "\"handlerAddr\":" << hexW(pipes.EXtoLS[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.EXtoLS.size() - 1)
      ss << ",";
  }
  ss << "],";
  
  // LStoEX
  ss << "\"LStoEX\":[";
  for (size_t i = 0; i < pipes.LStoEX.size(); ++i) {
    ss << "{";
    ss << "\"data\":[";
    for (size_t j = 0; j < pipes.LStoEX[i].data.size(); ++j) {
        ss << hexW(pipes.LStoEX[i].data[j]);
        if (j < pipes.LStoEX[i].data.size() - 1)
          ss << "\n";
    }
    ss << "],";
    ss << "\"sentAt\":" << pipes.LStoEX[i].sentAt << ",";
    ss << "\"associatedIP\":" << hexW(pipes.LStoEX[i].associatedIP, "#") << ",";
    ss << "\"exceptionTriggered\":" << (pipes.LStoEX[i].exceptionTriggered ? "true" : "false");
    if (pipes.LStoEX[i].exceptionTriggered) {
        ss << ",\"excpData\":" << EXCEPTION_HANDLER_NAMES[pipes.LStoEX[i].handlerAddr] << ",";
        ss << "\"handlerAddr\":" << hexW(pipes.LStoEX[i].handlerAddr, "#");
    }
    ss << "}";
    if (i < pipes.LStoEX.size() - 1)
      ss << ",";
  }
  ss << "]";
  
  ss << "},"; // End of pipes section
  
  // LS stage
  ss << "\"LS\":{";
  ss << "\"state\":\"" << LS.state << "\",";
  
  // LS cache
  ss << "\"cache\":{";
  ss << "\"size\":" << LS.cache.size << ",";
  ss << "\"storage\":[";
  for (unsigned int i = 0; i < LS.cache.size; ++i) {
    ss << "["; // Start of a cache set
    for (size_t j = 0; j < LS.cache.storage[i].storedLines.size(); ++j) {
        ss << "{";
        ss << "\"data\":" << hexW(LS.cache.storage[i].storedLines[j].data) << ",";
        ss << "\"tag\":" << hexW(LS.cache.storage[i].storedLines[j].tag, "#") << ",";
        ss << "\"lastHitTime\":" << LS.cache.storage[i].storedLines[j].lastHitTime << ",";
        ss << "\"valid\":" << (LS.cache.storage[i].storedLines[j].valid ? "true" : "false") << ",";
        ss << "\"modified\":" << (LS.cache.storage[i].storedLines[j].modified ? "true" : "false");
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
  
  ss << ",\"extra\":\"" << LS.extra << "\""; // Extra information for LS stage
  ss << "},"; // End of LS stage
  
  // IC stage
  ss << "\"IC\":{";
  ss << "\"state\":\"" << IC.state << "\",";
  ss << "\"internalIP\":" << hexW(IC.internalIP, "#") << ",";
  
  // IC cache
  ss << "\"cache\":{";
  ss << "\"storage\":[";
  for (unsigned int i = 0; i < IC.cache.size; ++i) {
    ss << "{";
    ss << "\"data\":" << hexFW(IC.cache.storage[i].data, "", {"", ""}) << ",";
    ss << "\"tag\": \"" << convDecToHex(IC.cache.storage[i].tag) << "\",";
    ss << "\"valid\":" << (IC.cache.storage[i].valid ? "true" : "false");
    ss << "}";
    if (i < IC.cache.size - 1)
      ss << ",";
  }
  ss << "]"; // End of IC cache storage
  ss << "}"; // End of IC cache
  
  ss << ",\"extra\":\"" << IC.extra << "\""; // Extra information for IC stage
  ss << "},"; // End of IC stage
  
  // DE stage
  ss << "\"DE\":{";
  ss << "\"state\":\"" << DE.state << "\",";
  
  // DE fw temp storage
  ss << "\"fwTempStorage\":{";
  ss << "\"storedFWs\":\"[";
  for (int i = 0; i < DE_WORK_MEMORY_FW_SIZE; ++i) {
    ss << hexFW(DE.fwTempStorage.storedFWs[i], "_", {"", ""}, false);
    if (i < DE_WORK_MEMORY_FW_SIZE - 1)
      ss << "_";
  }
  ss << "]\",";
  ss << "\"cacheStartAddr\":" << hexW(DE.fwTempStorage.cacheStartAddr, "#") << ",";
  ss << "\"storedWordsCount\":" << std::to_string(DE.fwTempStorage.storedWordsCount);
  ss << "}"; // End of DE fw temp storage
  
  ss << ",\"lastDecodedInstr\":\"" << DE.lastDecodedInstr << "\"";
  ss << ",\"extra\":\"" << DE.extra << "\""; // Extra information for DE stage
  ss << "},"; // End of DE stage

  // EX stage
  ss << "\"EX\":{";
  ss << "\"state\":\"" << EX.state << "\"";
  ss << ",\"activeException\":\"" << EX.activeException << "\"";
  ss << ",\"extra\":\"" << EX.extra << "\""; // Extra information for EX stage
  ss << "}"; // End of EX stage
  ss << "}"; // End of JSON object
  
  return ss.str();
}