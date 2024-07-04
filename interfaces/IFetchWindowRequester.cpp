#include "LoadStore.h"
#include "Config.h"

class IFetchWindowRequester
{
protected:
    LoadStore* const LSModule;

public:
    IFetchWindowRequester(LoadStore* lsModule): LSModule(lsModule) {};
    virtual void requestFetchWindow(address addr) = 0;
};