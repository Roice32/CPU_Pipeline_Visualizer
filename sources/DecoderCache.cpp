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
            storedFWs[fwIndex - 1] <<= 16;
            storedFWs[fwIndex - 1] |= storedFWs[fwIndex] >> 48;
        }
        storedFWs[DECODER_CACHE_FW_SIZE - 1] <<= 16;
        cacheStartAddr += 2;
    }
    storedWordsCount = (storedWordsCount - wordsCount > 0) ? (storedWordsCount - wordsCount) : 0;
    return *this;
}

void DecoderCache::concatNewFW(fetch_window newFW)
{
    assert(storedWordsCount < 4 && "Attempt to overwrite non-empty cache end");
    byte emptyWordsInFirstFW = 4 - storedWordsCount;
    storedFWs[0] >>= (emptyWordsInFirstFW * 16);
    storedFWs[1] = newFW;
    storedWordsCount += 4;
    cacheStartAddr -= emptyWordsInFirstFW * 2;
    *this << emptyWordsInFirstFW;
}

bool DecoderCache::reqIPAlreadyCached(address reqIP)
{
    return cacheStartAddr <= reqIP && reqIP <= (cacheStartAddr + (storedWordsCount - 1) * 2); 
}

bool DecoderCache::canProvideFullInstruction()
{
    byte neededWordsCount = 1;
    byte src1 = (storedFWs[0] >> 53) & 0b11111;
    byte src2 = (storedFWs[0] >> 48) & 0b11111;
    if (src1 == IMM || src1 == ADDR)
        ++neededWordsCount;
    if (src2 == IMM || src2 == ADDR)
        ++neededWordsCount;
    return storedWordsCount >= neededWordsCount;
}

void DecoderCache::bringIPToStart(address reqIP)
{
    while (cacheStartAddr != reqIP)
        *this << 1;
}

fetch_window DecoderCache::getFullInstrFetchWindow()
{
    return storedFWs[0];
}

void DecoderCache::shiftUsedWords(byte usedWordsCount)
{
    *this << usedWordsCount;
}

void DecoderCache::overwriteCache(fetch_window newFW, address newAddr)
{
    storedFWs[0] = newFW;
    storedFWs[1] = 0;
    cacheStartAddr = newAddr;
    storedWordsCount = 4;
}