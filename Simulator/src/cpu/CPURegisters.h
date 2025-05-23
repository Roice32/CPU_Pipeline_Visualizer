#pragma once

#include "Config.h"

#include <memory>
#include <vector>

class CPURegisters
{
public:
  std::shared_ptr<register_16b> registers[REGISTER_COUNT];
  std::shared_ptr<std::vector<word>> zRegisters[Z_REGISTER_COUNT];
  std::shared_ptr<register_16b> IP;
  std::shared_ptr<register_16b> flags;
  std::shared_ptr<register_16b> stackBase;
  std::shared_ptr<register_16b> stackSize;
  std::shared_ptr<register_16b> stackPointer;

  CPURegisters();
  void reset();
};