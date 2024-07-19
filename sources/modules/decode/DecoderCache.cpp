#include "DecoderCache.h"

#include <cassert>

DecoderCache::DecoderCache()
{
    for(byte ind = 0; ind < DECODER_CACHE_FW_SIZE; ++ind)
        storedFWs[ind] = 0;
    cacheStartAddr = 0;
    storedWordsCount = 0;
}

DecoderCache& DecoderCache::operator<<(const byte wordsCount)
{
    for (byte step = 0; step < wordsCount; ++step)
    {
        for (byte fwIndex = 1; fwIndex < DECODER_CACHE_FW_SIZE; ++fwIndex)
        {
            storedFWs[fwIndex - 1] <<= WORD_BYTES * 8;
            storedFWs[fwIndex - 1] |= storedFWs[fwIndex] >> ((FETCH_WINDOW_BYTES - WORD_BYTES) * 8);
        }
        storedFWs[DECODER_CACHE_FW_SIZE - 1] <<= WORD_BYTES * 8;
        cacheStartAddr += WORD_BYTES;
    }
    storedWordsCount = (storedWordsCount - wordsCount > 0) ? (storedWordsCount - wordsCount) : 0;
    return *this;
}

void DecoderCache::concatNewFW(fetch_window newFW)
{
    assert(storedWordsCount < FETCH_WINDOW_BYTES / WORD_BYTES && "Attempt to overwrite non-empty cache end");
    byte emptyWordsInFirstFW = FETCH_WINDOW_BYTES / WORD_BYTES - storedWordsCount;
    storedFWs[0] >>= (emptyWordsInFirstFW * WORD_BYTES * 8);
    storedFWs[1] = newFW;
    storedWordsCount += FETCH_WINDOW_BYTES / WORD_BYTES + emptyWordsInFirstFW;
    cacheStartAddr -= emptyWordsInFirstFW * WORD_BYTES;
    *this << emptyWordsInFirstFW;
}

void DecoderCache::overwriteCache(fetch_window newFW, address associatedIP)
{
    storedFWs[0] = storedFWs[1] = 0;
    storedFWs[0] = newFW;
    cacheStartAddr = associatedIP;
    storedWordsCount = 4;
}

bool DecoderCache::canProvideFullInstruction()
{
    byte neededWordsCount = 1;
    byte src1 = (storedFWs[0] >> (FETCH_WINDOW_BYTES * 8 - 11)) & 0b11111;
    byte src2 = (storedFWs[0] >> (FETCH_WINDOW_BYTES * 8 - 16)) & 0b11111;
    if (src1 == IMM || src1 == ADDR)
        ++neededWordsCount;
    if (src2 == IMM || src2 == ADDR)
        ++neededWordsCount;
    return storedWordsCount >= neededWordsCount;
}

fetch_window DecoderCache::getFullInstrFetchWindow()
{
    return storedFWs[0];
}

address DecoderCache::getAssociatedInstrAddr()
{
    return cacheStartAddr;
}

byte DecoderCache::getStoredWordsCount()
{
    return storedWordsCount;
}

void DecoderCache::shiftUsedWords(byte usedWordsCount)
{
    *this << usedWordsCount;
}

void DecoderCache::discardCurrent()
{
    storedFWs[0] = storedFWs[1] = 0;
    storedWordsCount = 0;
}