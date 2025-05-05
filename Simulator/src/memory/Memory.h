#pragma once

#include "Config.h"

#include <string>
#include <unordered_map>

class Memory
{
private:
  std::unordered_map<address, byte> data;

  bool isValidInputLine(std::string inLine);
  static byte hexCharToDec(const char digit);
  static void jumpToNewAddr(address& currAddr, const char* newAddr);
  void storeData(address& currAddr, const char* instr);

public:
  Memory(const char* hexSourceFilePath);
  byte getMemoryCell(address addr);
  void setMemoryCell(address addr, byte value);
  std::unordered_map<address, byte> getMemory() const;
  void dumpMemoryContents(const char* outputFile);
};