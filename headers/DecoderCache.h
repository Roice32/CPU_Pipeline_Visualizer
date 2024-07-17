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
    bool canProvideFullInstruction();
    fetch_window getFullInstrFetchWindow();
    address getAssociatedInstrAddr();
    void shiftUsedWords(byte usedWordsCount);
    void discardCurrent();
};