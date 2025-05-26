#include "ExecutionState.h"

std::string ExecutionState::toJSON() const
{
  std::stringstream ss;
  
  // Start of JSON object
  ss << "{";
  
  // Basic execution state properties
  ss << "\"cycle\":" << cycle << ",";
  
  // Registers section
  ss << "\"registers\":{";
  
  // Regular registers (R0-R7)
  ss << "\"R\":[";
  for (int i = 0; i < REGISTER_COUNT; ++i) {
    ss << "\"0x" + convDecToHex(registers.R[i]) + "\"";
    if (i < REGISTER_COUNT - 1) ss << ",";
  }
  ss << "],";
  
  // Vector registers (Z0-Z3)
  ss << "\"Z\":[";
  for (int i = 0; i < Z_REGISTER_COUNT; ++i) {
    ss << "[";
    for (int j = 0; j < WORDS_PER_Z_REGISTER; ++j) {
        ss << "\"0x" + convDecToHex(registers.Z[i][j]) + "\"";
        if (j < WORDS_PER_Z_REGISTER - 1) ss << ",";
    }
    ss << "]";
    if (i < Z_REGISTER_COUNT - 1) ss << ",";
  }
  ss << "],";
  
  // Special registers
  ss << "\"IP\":" << registers.IP << ",";
  ss << "\"flags\":" << registers.flags << ",";
  ss << "\"stackBase\":" << registers.stackBase << ",";
  ss << "\"stackSize\":" << registers.stackSize << ",";
  ss << "\"stackPointer\":" << registers.stackPointer;
  
  ss << "},"; // End of registers section
  
  // Stack section
  ss << "\"stack\":[";
  std::stack<word> stackCopy = stack;
  std::vector<word> stackItems;
  while (!stackCopy.empty()) {
    stackItems.push_back(stackCopy.top());
    stackCopy.pop();
  }
  // We need to reverse the order to match the actual stack order
  for (int i = stackItems.size() - 1; i >= 0; --i) {
    ss << stackItems[i];
    if (i > 0) ss << ",";
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
    ss << "\"data\": \"#" << convDecToHex(pipes.ICtoLS[i].data) << "\",";
    ss << "\"sentAt\":" << pipes.ICtoLS[i].sentAt << ",";
    ss << "\"associatedIP\": \"#" << convDecToHex(pipes.ICtoLS[i].associatedIP) << "\",";
    ss << "\"exceptionTriggered\":" << (pipes.ICtoLS[i].exceptionTriggered ? "true" : "false");
    if (pipes.ICtoLS[i].exceptionTriggered) {
      ss << ",\"excpData\":" << pipes.ICtoLS[i].excpData << ",";
      ss << "\"handlerAddr\":" << pipes.ICtoLS[i].handlerAddr;
    }
    ss << "}";
    if (i < pipes.ICtoLS.size() - 1) ss << ",";
  }
  ss << "],";
  
  // LStoIC
  ss << "\"LStoIC\":[";
  for (size_t i = 0; i < pipes.LStoIC.size(); ++i) {
    ss << "{";
    ss << "\"data\":" << pipes.LStoIC[i].data << ",";
    ss << "\"sentAt\":" << pipes.LStoIC[i].sentAt << ",";
    ss << "\"associatedIP\":" << pipes.LStoIC[i].associatedIP << ",";
    ss << "\"exceptionTriggered\":" << (pipes.LStoIC[i].exceptionTriggered ? "true" : "false");
    if (pipes.LStoIC[i].exceptionTriggered) {
        ss << ",\"excpData\":" << pipes.LStoIC[i].excpData << ",";
        ss << "\"handlerAddr\":" << pipes.LStoIC[i].handlerAddr;
    }
    ss << "}";
    if (i < pipes.LStoIC.size() - 1) ss << ",";
  }
  ss << "],";
  
  // ICtoDE
  ss << "\"ICtoDE\":[";
  for (size_t i = 0; i < pipes.ICtoDE.size(); ++i) {
    ss << "{";
    ss << "\"data\": \"" << fwToStr(pipes.ICtoDE[i].data) << "\",";
    ss << "\"sentAt\":" << pipes.ICtoDE[i].sentAt << ",";
    ss << "\"associatedIP\": \"#" << convDecToHex(pipes.ICtoDE[i].associatedIP) << "\",";
    ss << "\"exceptionTriggered\":" << (pipes.ICtoDE[i].exceptionTriggered ? "true" : "false");
    if (pipes.ICtoDE[i].exceptionTriggered) {
        ss << ",\"excpData\":" << pipes.ICtoDE[i].excpData << ",";
        ss << "\"handlerAddr\":" << pipes.ICtoDE[i].handlerAddr;
    }
    ss << "}";
    if (i < pipes.ICtoDE.size() - 1) ss << ",";
  }
  ss << "],";
  
  // DEtoIC
  ss << "\"DEtoIC\":[";
  for (size_t i = 0; i < pipes.DEtoIC.size(); ++i) {
    ss << "{";
    ss << "\"data\": \"#" << convDecToHex(pipes.DEtoIC[i].data) << "\",";
    ss << "\"sentAt\":" << pipes.DEtoIC[i].sentAt << ",";
    ss << "\"associatedIP\": \"#" << convDecToHex(pipes.DEtoIC[i].associatedIP) << "\",";
    ss << "\"exceptionTriggered\":" << (pipes.DEtoIC[i].exceptionTriggered ? "true" : "false");
    if (pipes.DEtoIC[i].exceptionTriggered) {
        ss << ",\"excpData\":" << pipes.DEtoIC[i].excpData << ",";
        ss << "\"handlerAddr\":" << pipes.DEtoIC[i].handlerAddr;
    }
    ss << "}";
    if (i < pipes.DEtoIC.size() - 1) ss << ",";
  }
  ss << "],";
  
  // DEtoEX
  ss << "\"DEtoEX\":[";
  for (size_t i = 0; i < pipes.DEtoEX.size(); ++i) {
    ss << "{";
    ss << "\"data\":{";
    ss << "\"opCode\":" << static_cast<int>(pipes.DEtoEX[i].data.opCode) << ",";
    ss << "\"src1\":" << static_cast<int>(pipes.DEtoEX[i].data.src1) << ",";
    ss << "\"src2\":" << static_cast<int>(pipes.DEtoEX[i].data.src2) << ",";
    ss << "\"param1\":" << pipes.DEtoEX[i].data.param1 << ",";
    ss << "\"param2\":" << pipes.DEtoEX[i].data.param2;
    ss << "},";
    ss << "\"sentAt\":" << pipes.DEtoEX[i].sentAt << ",";
    ss << "\"associatedIP\":" << pipes.DEtoEX[i].associatedIP << ",";
    ss << "\"exceptionTriggered\":" << (pipes.DEtoEX[i].exceptionTriggered ? "true" : "false");
    if (pipes.DEtoEX[i].exceptionTriggered) {
        ss << ",\"excpData\":" << pipes.DEtoEX[i].excpData << ",";
        ss << "\"handlerAddr\":" << pipes.DEtoEX[i].handlerAddr;
    }
    ss << "}";
    if (i < pipes.DEtoEX.size() - 1) ss << ",";
  }
  ss << "],";
  
  // EXtoDE
  ss << "\"EXtoDE\":[";
  for (size_t i = 0; i < pipes.EXtoDE.size(); ++i) {
    ss << "{";
    ss << "\"data\":" << pipes.EXtoDE[i].data << ",";
    ss << "\"sentAt\":" << pipes.EXtoDE[i].sentAt << ",";
    ss << "\"associatedIP\":" << pipes.EXtoDE[i].associatedIP << ",";
    ss << "\"exceptionTriggered\":" << (pipes.EXtoDE[i].exceptionTriggered ? "true" : "false");
    if (pipes.EXtoDE[i].exceptionTriggered) {
        ss << ",\"excpData\":" << pipes.EXtoDE[i].excpData << ",";
        ss << "\"handlerAddr\":" << pipes.EXtoDE[i].handlerAddr;
    }
    ss << "}";
    if (i < pipes.EXtoDE.size() - 1) ss << ",";
  }
  ss << "],";
  
  // EXtoLS
  ss << "\"EXtoLS\":[";
  for (size_t i = 0; i < pipes.EXtoLS.size(); ++i) {
    ss << "{";
    ss << "\"data\":{";
    ss << "\"reqAddr\":" << pipes.EXtoLS[i].data.reqAddr << ",";
    ss << "\"wordsSizeOfReq\":" << static_cast<int>(pipes.EXtoLS[i].data.wordsSizeOfReq) << ",";
    ss << "\"isStoreOperation\":" << (pipes.EXtoLS[i].data.isStoreOperation ? "true" : "false") << ",";
    ss << "\"reqData\":[";
    for (size_t j = 0; j < pipes.EXtoLS[i].data.reqData.size(); ++j) {
        ss << pipes.EXtoLS[i].data.reqData[j];
        if (j < pipes.EXtoLS[i].data.reqData.size() - 1) ss << ",";
    }
    ss << "]";
    ss << "},";
    ss << "\"sentAt\":" << pipes.EXtoLS[i].sentAt << ",";
    ss << "\"associatedIP\":" << pipes.EXtoLS[i].associatedIP << ",";
    ss << "\"exceptionTriggered\":" << (pipes.EXtoLS[i].exceptionTriggered ? "true" : "false");
    if (pipes.EXtoLS[i].exceptionTriggered) {
        ss << ",\"excpData\":" << pipes.EXtoLS[i].excpData << ",";
        ss << "\"handlerAddr\":" << pipes.EXtoLS[i].handlerAddr;
    }
    ss << "}";
    if (i < pipes.EXtoLS.size() - 1) ss << ",";
  }
  ss << "],";
  
  // LStoEX
  ss << "\"LStoEX\":[";
  for (size_t i = 0; i < pipes.LStoEX.size(); ++i) {
    ss << "{";
    ss << "\"data\":[";
    for (size_t j = 0; j < pipes.LStoEX[i].data.size(); ++j) {
        ss << pipes.LStoEX[i].data[j];
        if (j < pipes.LStoEX[i].data.size() - 1) ss << ",";
    }
    ss << "],";
    ss << "\"sentAt\":" << pipes.LStoEX[i].sentAt << ",";
    ss << "\"associatedIP\":" << pipes.LStoEX[i].associatedIP << ",";
    ss << "\"exceptionTriggered\":" << (pipes.LStoEX[i].exceptionTriggered ? "true" : "false");
    if (pipes.LStoEX[i].exceptionTriggered) {
        ss << ",\"excpData\":" << pipes.LStoEX[i].excpData << ",";
        ss << "\"handlerAddr\":" << pipes.LStoEX[i].handlerAddr;
    }
    ss << "}";
    if (i < pipes.LStoEX.size() - 1) ss << ",";
  }
  ss << "]";
  
  ss << "},"; // End of pipes section
  
  // LS stage
  ss << "\"LS\":{";
  ss << "\"state\":\"" << LS.state << "\",";
  ss << "\"physicalMemoryAccessHappened\":" << (LS.physicalMemoryAccessHappened ? "true" : "false") << ",";
  
  // LS cache
  ss << "\"cache\":{";
  ss << "\"size\":" << LS.cache.size << ",";
  ss << "\"storage\":[";
  for (unsigned int i = 0; i < LS.cache.size; ++i) {
    ss << "["; // Start of a cache set
    for (size_t j = 0; j < LS.cache.storage[i].storedLines.size(); ++j) {
        ss << "{";
        ss << "\"data\": \"0x" << convDecToHex(LS.cache.storage[i].storedLines[j].data) << "\",";
        ss << "\"tag\": \"" << convDecToHex(LS.cache.storage[i].storedLines[j].tag) << "\",";
        ss << "\"lastHitTime\":" << LS.cache.storage[i].storedLines[j].lastHitTime << ",";
        ss << "\"valid\":" << (LS.cache.storage[i].storedLines[j].valid ? "true" : "false") << ",";
        ss << "\"modified\":" << (LS.cache.storage[i].storedLines[j].modified ? "true" : "false");
        ss << "}";
        if (j < LS.cache.storage[i].storedLines.size() - 1) ss << ",";
    }
    ss << "]"; // End of a cache set
    if (i < LS.cache.size - 1) ss << ",";
  }
  ss << "],";
  ss << "\"currReqIndex\":" << static_cast<int>(LS.cache.currReqIndex) << ",";
  ss << "\"currReqTag\":" << LS.cache.currReqTag << ",";
  ss << "\"foundIndex\":" << static_cast<int>(LS.cache.foundIndex);
  ss << "}"; // End of LS cache
  
  ss << ",\"extra\":\"" << LS.extra << "\""; // Extra information for LS stage
  ss << "},"; // End of LS stage
  
  // IC stage
  ss << "\"IC\":{";
  ss << "\"state\":\"" << IC.state << "\",";
  ss << "\"internalIP\":" << IC.internalIP << ",";
  
  // IC cache
  ss << "\"cache\":{";
  ss << "\"size\":" << IC.cache.size << ",";
  ss << "\"storage\":[";
  for (unsigned int i = 0; i < IC.cache.size; ++i) {
    ss << "{";
    ss << "\"data\": \"" << fwToStr(IC.cache.storage[i].data) << "\",";
    ss << "\"tag\": \"" << convDecToHex(IC.cache.storage[i].tag) << "\",";
    ss << "\"valid\":" << (IC.cache.storage[i].valid ? "true" : "false");
    ss << "}";
    if (i < IC.cache.size - 1) ss << ",";
  }
  ss << "],";
  ss << "\"currReqIndex\":" << static_cast<int>(IC.cache.currReqIndex) << ",";
  ss << "\"currReqTag\":" << static_cast<int>(IC.cache.currReqTag);
  ss << "}"; // End of IC cache
  
  ss << ",\"extra\":\"" << IC.extra << "\""; // Extra information for IC stage
  ss << "},"; // End of IC stage
  
  // DE stage
  ss << "\"DE\":{";
  ss << "\"state\":\"" << DE.state << "\",";
  
  // DE fw temp storage
  ss << "\"fwTempStorage\":{";
  ss << "\"storedFWs\":[";
  for (int i = 0; i < DE_WORK_MEMORY_FW_SIZE; ++i) {
    ss << "\"" << fwToStr(DE.fwTempStorage.storedFWs[i]) << "\"";
    if (i < DE_WORK_MEMORY_FW_SIZE - 1) ss << ",";
  }
  ss << "],";
  ss << "\"cacheStartAddr\":" << DE.fwTempStorage.cacheStartAddr << ",";
  ss << "\"storedWordsCount\":" << static_cast<int>(DE.fwTempStorage.storedWordsCount);
  ss << "}"; // End of DE fw temp storage
  
  ss << ",\"lastDecodedInstr\":\"" << DE.lastDecodedInstr << "\"";
  ss << ",\"extra\":\"" << DE.extra << "\""; // Extra information for DE stage
  ss << "},"; // End of DE stage
  
  // EX stage
  ss << "\"EX\":{";
  ss << "\"state\":\"" << EX.state << "\"";
  ss << "}"; // End of EX stage
  ss << ",\"extra\":\"" << EX.extra << "\""; // Extra information for EX stage
  ss << "}"; // End of JSON object
  
  return ss.str();
}