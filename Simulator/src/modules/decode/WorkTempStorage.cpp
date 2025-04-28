#include "WorkTempStorage.h"

#include <cassert>

WorkTempStorage::WorkTempStorage()
{
    for(byte ind = 0; ind < DE_WORK_MEMORY_FW_SIZE; ++ind)
        storedFWs[ind] = 0;
    cacheStartAddr = 0;
    storedWordsCount = 0;
}

WorkTempStorage& WorkTempStorage::operator<<(const byte wordsCount)
{
    for (byte step = 0; step < wordsCount; ++step)
    {
        for (byte fwIndex = 1; fwIndex < DE_WORK_MEMORY_FW_SIZE; ++fwIndex)
        {
            storedFWs[fwIndex - 1] <<= WORD_BYTES * 8;
            storedFWs[fwIndex - 1] |= storedFWs[fwIndex] >> ((FETCH_WINDOW_BYTES - WORD_BYTES) * 8);
        }
        storedFWs[DE_WORK_MEMORY_FW_SIZE - 1] <<= WORD_BYTES * 8;
        cacheStartAddr += WORD_BYTES;
    }
    storedWordsCount = (storedWordsCount - wordsCount > 0) ? (storedWordsCount - wordsCount) : 0;
    return *this;
}

void WorkTempStorage::concatNewFW(fetch_window newFW)
{
    assert(storedWordsCount < FETCH_WINDOW_BYTES / WORD_BYTES && "Attempt to overwrite non-empty cache end");
    byte emptyWordsInFirstFW = FETCH_WINDOW_BYTES / WORD_BYTES - storedWordsCount;
    storedFWs[0] >>= (emptyWordsInFirstFW * WORD_BYTES * 8);
    storedFWs[1] = newFW;
    storedWordsCount += FETCH_WINDOW_BYTES / WORD_BYTES + emptyWordsInFirstFW;
    cacheStartAddr -= emptyWordsInFirstFW * WORD_BYTES;
    *this << emptyWordsInFirstFW;
}

void WorkTempStorage::overwriteCache(fetch_window newFW, address associatedIP)
{
    storedFWs[0] = storedFWs[1] = 0;
    storedFWs[0] = newFW;
    cacheStartAddr = associatedIP;
    storedWordsCount = 4;
}

bool WorkTempStorage::canProvideFullInstruction()
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

fetch_window WorkTempStorage::getFullInstrFetchWindow()
{
    return storedFWs[0];
}

address WorkTempStorage::getAssociatedInstrAddr()
{
    return cacheStartAddr;
}

byte WorkTempStorage::getStoredWordsCount()
{
    return storedWordsCount;
}

void WorkTempStorage::shiftUsedWords(byte usedWordsCount)
{
    *this << usedWordsCount;
}

void WorkTempStorage::discardCurrent()
{
    storedFWs[0] = storedFWs[1] = 0;
    storedWordsCount = 0;
}