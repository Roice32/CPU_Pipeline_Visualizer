#pragma once

#include "Config.h"

class DecoderCache
{
private:
    fetch_window storedFWs[DECODER_CACHE_FW_SIZE];
    address cacheStartAddr;
    byte storedWordsCount;

    DecoderCache& operator<<(const byte wordsCount);

public:
    DecoderCache();
    void concatNewFW(fetch_window newFW);
    bool reqIPAlreadyCached(address reqIP);
    bool canProvideFullInstruction();
    void bringIPToStart(address reqIP);
    fetch_window getFullInstrFetchWindow();
    void shiftUsedWords(byte usedWordsCount);
    void overwriteCache(fetch_window newFW, address newAddr);
};