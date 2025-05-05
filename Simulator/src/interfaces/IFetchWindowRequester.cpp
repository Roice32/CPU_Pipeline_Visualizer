#pragma once

#include "LoadStore.h"

class IFetchWindowRequester
{
protected:
  std::shared_ptr<LoadStore> const LSModule;
  
  IFetchWindowRequester(std::shared_ptr<LoadStore> lsModule): LSModule(lsModule) {};
  virtual void requestFetchWindow() = 0;
};