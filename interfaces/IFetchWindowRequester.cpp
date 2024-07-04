#include "LoadStore.h"
#include "Config.h"

class IFetchWindowRequester
{
protected:
    LoadStore* const LSModule;

public:
    IFetchWindowRequester(LoadStore* lsModule): LSModule(lsModule) {};
    virtual fetch_window requestFetchWindow(address addr) = 0;
};