#pragma once

#include "Config.h"

class ICLogPackage
{
public:
    address ip;
    fetch_window fetchWindow;

    ICLogPackage(address ip, fetch_window fw):
        ip(ip), fetchWindow(fw) {};
};