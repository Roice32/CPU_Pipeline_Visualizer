#pragma once

#include <iostream>
#include <fstream>
#include <map>

#include "ExecutionState.h"

enum Modules
{
  IC,
  LS,
  DE,
  EX
};

enum Pipes
{
  ICtoLS,
  LStoIC,
  ICtoDE,
  DEtoIC,
  DEtoEX,
  EXtoDE,
  EXtoLS,
  LStoEX
};

class ExecutionRecorder
{
private:
  std::map<address, word> memory = {};

  std::vector<ExecutionState> states = {};
  void dumpStateToJSON(const ExecutionState& state, const std::string& outputDirPath);
  void updateMemory(const std::unordered_map<address, word>& memoryChanges);
  void dumpMemoryToJSON(const clock_time cycle, const std::string& outputDirPath);

public:
  ExecutionRecorder(std::shared_ptr<Memory> mem);
  void goToNextState();
  void modifyModuleState(const Modules& moduleName, const std::string& state);
  void addExtraInfo(const Modules& moduleName, const std::string& extraInfo);

  inline void pushICtoLSData(const SynchronizedDataPackage<address>& data)
    { states.back().pipes.ICtoLS.push_back(data); }
  inline void pushLStoICData(const SynchronizedDataPackage<fetch_window>& data)
    { states.back().pipes.LStoIC.push_back(data); }
  inline void pushICtoDEData(const SynchronizedDataPackage<fetch_window>& data)
    { states.back().pipes.ICtoDE.push_back(data); }
  inline void pushDEtoICData(const SynchronizedDataPackage<address>& data)
    { states.back().pipes.DEtoIC.push_back(data); }
  inline void pushDEtoEXData(const SynchronizedDataPackage<Instruction>& data)
    { states.back().pipes.DEtoEX.push_back(data); }
  inline void pushEXtoDEData(const SynchronizedDataPackage<address>& data)
    { states.back().pipes.EXtoDE.push_back(data); }
  inline void pushEXtoLSData(const SynchronizedDataPackage<MemoryAccessRequest>& data)
    { states.back().pipes.EXtoLS.push_back(data); }
  inline void pushLStoEXData(const SynchronizedDataPackage<std::vector<word>>& data)
    { states.back().pipes.LStoEX.push_back(data); }

  void popPipeData(const Pipes& pipeName);
  void invalidateICCacheLine(const byte& index);
  void swapICCacheLine(const CacheLine<fetch_window>& newLine, const byte& index);
  void modifICInternalIP(const address& newIP)
    { states.back().IC.internalIP = newIP; }
  
  void lastDecodedInstruction(const std::string& instr)
    { states.back().DE.lastDecodedInstr = instr; }
  void rewriteDEWorkTempStorage(const fetch_window* fws,
                                const address& cacheStartAddr,
                                const byte& storedWordsCount);

  void dumpSimulationToJSONs(const std::string& outputDirPath);
};