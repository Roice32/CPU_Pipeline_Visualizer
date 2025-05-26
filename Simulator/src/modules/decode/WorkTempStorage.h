#pragma once

#include "Config.h"

class WorkTempStorage
{
private:
  fetch_window storedFWs[DE_WORK_MEMORY_FW_SIZE];
  address cacheStartAddr;
  byte storedWordsCount;

  WorkTempStorage& operator<<(const byte wordsCount);

public:
  WorkTempStorage();
  void concatNewFW(fetch_window newFW);
  void overwriteCache(fetch_window newFW, address associatedIP);
  bool canProvideFullInstruction();
  fetch_window getFullInstrFetchWindow();
  address getAssociatedInstrAddr();
  byte getStoredWordsCount();
  void shiftUsedWords(byte usedWordsCount);
  void discardCurrent();

friend class Decode;
};