#pragma once

#include "Config.h"
#include "ParameterMetadata.h"

class InstructionMetadata
{
public:
    const char* name;
    const byte paramsCount;
    const ParameterMetadata* const params;

    InstructionMetadata();
    ~InstructionMetadata()
    {
        delete[] name;
        delete[] params;
    }
};